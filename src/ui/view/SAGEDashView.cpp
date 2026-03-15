
// SAGEDashView.cpp: CSAGEDashView 클래스의 구현

#include "pch.h"
#include "framework.h"
#include "SAGEDash.h"
#include "SAGEDashDoc.h"
#include "SAGEDashView.h"
#include "TabularData.h"
#include "Define.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define IDC_SEARCH_EDIT      100
#define IDC_MAPPING_PANEL    110
#define IDC_VALIDATION_PANEL 111

IMPLEMENT_DYNCREATE(CSAGEDashView, CView)

BEGIN_MESSAGE_MAP(CSAGEDashView, CView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_EN_CHANGE(IDC_SEARCH_EDIT, OnEnChangeSearch)
END_MESSAGE_MAP()

CSAGEDashView::CSAGEDashView() noexcept
	: m_eViewMode(VIEW_MODE_GRID)
{
}

CSAGEDashView::~CSAGEDashView()
{
}

int CSAGEDashView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_edtSearch.Create(
			WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
			rectDummy, this, IDC_SEARCH_EDIT)) {
		TRACE0("검색 컨트롤을 만들지 못했습니다.\n");
		return -1;
	}

	if (!m_lstGrid.Create(
			WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER,
			rectDummy, this, 2)) {
		TRACE0("그리드 컨트롤을 만들지 못했습니다.\n");
		return -1;
	}
	m_lstGrid.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	if (!m_wndMapping.Create(this, IDC_MAPPING_PANEL)) {
		TRACE0("매핑 패널을 만들지 못했습니다.\n");
		return -1;
	}

	if (!m_wndValidation.Create(this, IDC_VALIDATION_PANEL)) {
		TRACE0("검증 패널을 만들지 못했습니다.\n");
		return -1;
	}

	return 0;
}

void CSAGEDashView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_edtSearch.GetSafeHwnd() == nullptr)
		return;

	UpdateLayout(cx, cy);
}

void CSAGEDashView::UpdateLayout(int cx, int cy)
{
	BOOL bGrid       = (m_eViewMode == VIEW_MODE_GRID);
	BOOL bMapping    = (m_eViewMode == VIEW_MODE_MAPPING);
	BOOL bValidation = (m_eViewMode == VIEW_MODE_VALIDATION);

	// Grid 모드: 검색바 + 그리드
	if (m_edtSearch.GetSafeHwnd() != nullptr) {
		m_edtSearch.ShowWindow(bGrid ? SW_SHOW : SW_HIDE);
	}
	if (m_lstGrid.GetSafeHwnd() != nullptr) {
		m_lstGrid.ShowWindow(bGrid ? SW_SHOW : SW_HIDE);
	}

	// Mapping 모드: 매핑 패널 전체 영역
	if (m_wndMapping.GetSafeHwnd() != nullptr) {
		m_wndMapping.ShowWindow(bMapping ? SW_SHOW : SW_HIDE);
	}

	// Validation 모드: 검증 패널 전체 영역
	if (m_wndValidation.GetSafeHwnd() != nullptr) {
		m_wndValidation.ShowWindow(bValidation ? SW_SHOW : SW_HIDE);
	}

	// 각 모드별 위치/크기 설정
	if (bGrid) {
		int nSearchTop  = SEARCH_BAR_MARGIN;
		int nSearchH    = SEARCH_BAR_HEIGHT;
		int nGridTop    = nSearchTop + nSearchH + SEARCH_BAR_MARGIN;
		int nGridHeight = cy - nGridTop;
		if (nGridHeight < 0)
			nGridHeight = 0;

		if (m_edtSearch.GetSafeHwnd() != nullptr) {
			m_edtSearch.SetWindowPos(nullptr, SEARCH_BAR_MARGIN, nSearchTop,
				cx - SEARCH_BAR_MARGIN * 2, nSearchH, SWP_NOZORDER | SWP_NOACTIVATE);
		}
		if (m_lstGrid.GetSafeHwnd() != nullptr) {
			m_lstGrid.SetWindowPos(nullptr, 0, nGridTop,
				cx, nGridHeight, SWP_NOZORDER | SWP_NOACTIVATE);
		}
	} else if (bMapping && m_wndMapping.GetSafeHwnd() != nullptr) {
		m_wndMapping.SetWindowPos(nullptr, 0, 0,
			cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
	} else if (bValidation && m_wndValidation.GetSafeHwnd() != nullptr) {
		m_wndValidation.SetWindowPos(nullptr, 0, 0,
			cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
	}
}

void CSAGEDashView::SwitchViewMode(CenterViewMode eMode)
{
	if (m_eViewMode == eMode)
		return;

	m_eViewMode = eMode;

	if (eMode == VIEW_MODE_MAPPING) {
		CSAGEDashDoc* pDoc = GetDocument();
		if (pDoc != nullptr && pDoc->HasData()) {
			const DataSheet& sheet = pDoc->GetData().GetSheet(0);
			if (sheet.GetRowCount() > 0) {
				m_wndMapping.SetSourceColumns(sheet.m_arrRows[0]);
			}
		}
	}

	CRect rect;
	GetClientRect(&rect);
	UpdateLayout(rect.Width(), rect.Height());
}

void CSAGEDashView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	OnUpdate(nullptr, 0, nullptr);
}

void CSAGEDashView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	CSAGEDashDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	ClearGrid();
	m_wndMapping.ClearAll();

	if (m_edtSearch.GetSafeHwnd() != nullptr)
		m_edtSearch.SetWindowText(_T(""));

	if (pDoc == nullptr || !pDoc->HasData())
		return;

	// 파일을 새로 열면 Grid 모드로 전환
	if (m_eViewMode != VIEW_MODE_GRID) {
		SwitchViewMode(VIEW_MODE_GRID);
	}

	const DataSheet& sheet = pDoc->GetData().GetSheet(0);
	if (sheet.GetRowCount() == 0)
		return;

	PopulateGrid(sheet);
}

void CSAGEDashView::OnEnChangeSearch()
{
	CString strKeyword;
	m_edtSearch.GetWindowText(strKeyword);
	FilterGrid(strKeyword);
}

void CSAGEDashView::FilterGrid(const CString& strKeyword)
{
	CSAGEDashDoc* pDoc = GetDocument();
	if (pDoc == nullptr || !pDoc->HasData())
		return;

	const DataSheet& sheet = pDoc->GetData().GetSheet(0);
	if (sheet.GetRowCount() == 0)
		return;

	m_lstGrid.DeleteAllItems();

	if (strKeyword.IsEmpty()) {
		int nColCount = m_lstGrid.GetHeaderCtrl() ? m_lstGrid.GetHeaderCtrl()->GetItemCount() - 1 : 0;
		int nDataRows = min(sheet.GetRowCount() - 1, MAX_PREVIEW_ROWS);
		int nInsertIdx = 0;
		for (int nRow = 1; nRow <= nDataRows; nRow++) {
			const std::vector<CString>& row = sheet.m_arrRows[nRow];
			CString strRowNum;
			strRowNum.Format(_T("%d"), nRow);

			LVITEM lvi   = {};
			lvi.mask     = LVIF_TEXT;
			lvi.iItem    = nInsertIdx++;
			lvi.pszText  = (LPTSTR)(LPCTSTR)strRowNum;
			int nIdx = m_lstGrid.InsertItem(&lvi);

			for (int nCol = 0; nCol < nColCount; nCol++) {
				CString strCell = (nCol < (int)row.size()) ? row[nCol] : CString(_T(""));
				m_lstGrid.SetItemText(nIdx, nCol + 1, strCell);
			}
		}
		return;
	}

	CString strKeyLower = strKeyword;
	strKeyLower.MakeLower();

	int nColCount  = m_lstGrid.GetHeaderCtrl() ? m_lstGrid.GetHeaderCtrl()->GetItemCount() - 1 : 0;
	int nDataRows  = min(sheet.GetRowCount() - 1, MAX_PREVIEW_ROWS);
	int nInsertIdx = 0;

	for (int nRow = 1; nRow <= nDataRows; nRow++) {
		const std::vector<CString>& row = sheet.m_arrRows[nRow];
		BOOL bMatch = FALSE;
		for (int nCol = 0; nCol < (int)row.size(); nCol++) {
			CString strCell = row[nCol];
			strCell.MakeLower();
			if (strCell.Find(strKeyLower) >= 0) {
				bMatch = TRUE;
				break;
			}
		}
		if (!bMatch)
			continue;

		CString strRowNum;
		strRowNum.Format(_T("%d"), nRow);

		LVITEM lvi  = {};
		lvi.mask    = LVIF_TEXT;
		lvi.iItem   = nInsertIdx++;
		lvi.pszText = (LPTSTR)(LPCTSTR)strRowNum;
		int nIdx    = m_lstGrid.InsertItem(&lvi);

		for (int nCol = 0; nCol < nColCount; nCol++) {
			CString strCell = (nCol < (int)row.size()) ? row[nCol] : CString(_T(""));
			m_lstGrid.SetItemText(nIdx, nCol + 1, strCell);
		}
	}
}

void CSAGEDashView::PopulateGrid(const DataSheet& sheet)
{
	int nRowCount = sheet.GetRowCount();
	if (nRowCount == 0)
		return;

	LVCOLUMN lvc;
	CString strRowNumHeader;
	strRowNumHeader.LoadString(IDS_VIEW_GRID_ROW_NUM);
	lvc.mask    = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	lvc.fmt     = LVCFMT_RIGHT;
	lvc.cx      = GRID_ROW_NUM_COLUMN_WIDTH;
	lvc.pszText = (LPTSTR)(LPCTSTR)strRowNumHeader;
	m_lstGrid.InsertColumn(0, &lvc);

	const std::vector<CString>& headerRow = sheet.m_arrRows[0];
	int nColCount = (int)headerRow.size();

	for (int nCol = 0; nCol < nColCount; nCol++) {
		CString strHeader = headerRow[nCol];
		lvc.mask    = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
		lvc.fmt     = LVCFMT_LEFT;
		lvc.cx      = GRID_COLUMN_WIDTH_DEFAULT;
		lvc.pszText = (LPTSTR)(LPCTSTR)strHeader;
		m_lstGrid.InsertColumn(nCol + 1, &lvc);
	}

	int nDataRows = min(nRowCount - 1, MAX_PREVIEW_ROWS);
	for (int nRow = 1; nRow <= nDataRows; nRow++) {
		const std::vector<CString>& row = sheet.m_arrRows[nRow];
		int nCellCount = (int)row.size();

		CString strRowNum;
		strRowNum.Format(_T("%d"), nRow);

		LVITEM lvi;
		lvi.mask     = LVIF_TEXT;
		lvi.iItem    = nRow - 1;
		lvi.iSubItem = 0;
		lvi.pszText  = (LPTSTR)(LPCTSTR)strRowNum;
		int nInserted = m_lstGrid.InsertItem(&lvi);

		for (int nCol = 0; nCol < nColCount; nCol++) {
			CString strCell = (nCol < nCellCount) ? row[nCol] : CString(_T(""));
			m_lstGrid.SetItemText(nInserted, nCol + 1, strCell);
		}
	}

	for (int nCol = 1; nCol <= nColCount; nCol++) {
		m_lstGrid.SetColumnWidth(nCol, LVSCW_AUTOSIZE_USEHEADER);
		int nWidth = m_lstGrid.GetColumnWidth(nCol);
		if (nWidth > GRID_COLUMN_WIDTH_MAX)
			m_lstGrid.SetColumnWidth(nCol, GRID_COLUMN_WIDTH_MAX);
		else if (nWidth < GRID_COLUMN_WIDTH_MIN)
			m_lstGrid.SetColumnWidth(nCol, GRID_COLUMN_WIDTH_MIN);
	}
}

void CSAGEDashView::ClearGrid()
{
	m_lstGrid.DeleteAllItems();

	int nColCount = m_lstGrid.GetHeaderCtrl() ? m_lstGrid.GetHeaderCtrl()->GetItemCount() : 0;
	for (int i = nColCount - 1; i >= 0; i--)
		m_lstGrid.DeleteColumn(i);
}

void CSAGEDashView::OnDraw(CDC* /*pDC*/)
{
	// 그리기는 각 패널이 담당
}

#ifdef _DEBUG
void CSAGEDashView::AssertValid() const
{
	CView::AssertValid();
}

void CSAGEDashView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSAGEDashDoc* CSAGEDashView::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSAGEDashDoc)));
	return (CSAGEDashDoc*)m_pDocument;
}
#endif //_DEBUG
