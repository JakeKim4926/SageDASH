
// SAGEDashView.cpp: CSAGEDashView 클래스의 구현

#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "SAGEDash.h"
#endif

#include "SAGEDashDoc.h"
#include "SAGEDashView.h"
#include "Workbook.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 미리보기 최대 행 수 (헤더 제외)
static const int MAX_PREVIEW_ROWS = 500;

IMPLEMENT_DYNCREATE(CSAGEDashView, CView)

BEGIN_MESSAGE_MAP(CSAGEDashView, CView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
    ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CSAGEDashView::OnFilePrintPreview)
    ON_WM_CONTEXTMENU()
    ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

CSAGEDashView::CSAGEDashView() noexcept
{
}

CSAGEDashView::~CSAGEDashView()
{
}

BOOL CSAGEDashView::PreCreateWindow(CREATESTRUCT& cs)
{
    return CView::PreCreateWindow(cs);
}

int CSAGEDashView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    if (!m_lstGrid.Create(
            WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER,
            rectDummy, this, 1)) {
        TRACE0("그리드 컨트롤을 만들지 못했습니다.\n");
        return -1;
    }

    m_lstGrid.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    return 0;
}

void CSAGEDashView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    if (m_lstGrid.GetSafeHwnd() != nullptr)
        m_lstGrid.SetWindowPos(nullptr, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
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

    if (pDoc == nullptr || !pDoc->HasWorkbook())
        return;

    const CWorksheet& sheet = pDoc->GetWorkbook().GetSheet(0);
    if (sheet.GetRowCount() == 0)
        return;

    PopulateGrid(sheet);
}

void CSAGEDashView::PopulateGrid(const CWorksheet& sheet)
{
    int nRowCount = sheet.GetRowCount();
    if (nRowCount == 0)
        return;

    // 1행: 컬럼 헤더
    const std::vector<CString>& headerRow = sheet.m_arrRows[0];
    int nColCount = (int)headerRow.size();

    for (int nCol = 0; nCol < nColCount; nCol++) {
        CString strHeader = headerRow[nCol];
        LVCOLUMN lvc;
        lvc.mask    = LVCF_TEXT | LVCF_WIDTH;
        lvc.cx      = 120;
        lvc.pszText = (LPTSTR)(LPCTSTR)strHeader;
        m_lstGrid.InsertColumn(nCol, &lvc);
    }

    // 2행~: 데이터
    int nDataRows = min(nRowCount - 1, MAX_PREVIEW_ROWS);
    for (int nRow = 1; nRow <= nDataRows; nRow++) {
        const std::vector<CString>& row = sheet.m_arrRows[nRow];
        int nCellCount = (int)row.size();

        CString strFirst = (nCellCount > 0) ? row[0] : CString(_T(""));
        LVITEM lvi;
        lvi.mask     = LVIF_TEXT;
        lvi.iItem    = nRow - 1;
        lvi.iSubItem = 0;
        lvi.pszText  = (LPTSTR)(LPCTSTR)strFirst;
        int nInserted = m_lstGrid.InsertItem(&lvi);

        for (int nCol = 1; nCol < nColCount; nCol++) {
            CString strCell = (nCol < nCellCount) ? row[nCol] : CString(_T(""));
            m_lstGrid.SetItemText(nInserted, nCol, strCell);
        }
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
    // 그리기는 CListCtrl이 담당
}

// 인쇄

void CSAGEDashView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
    AFXPrintPreview(this);
#endif
}

BOOL CSAGEDashView::OnPreparePrinting(CPrintInfo* pInfo)
{
    return DoPreparePrinting(pInfo);
}

void CSAGEDashView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CSAGEDashView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CSAGEDashView::OnRButtonUp(UINT /*nFlags*/, CPoint point)
{
    ClientToScreen(&point);
    OnContextMenu(this, point);
}

void CSAGEDashView::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
#ifndef SHARED_HANDLERS
    theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
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
