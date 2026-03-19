
#include "pch.h"
#include "framework.h"
#include "PropertiesPane.h"
#include "Define.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

PropertiesPane::PropertiesPane() noexcept
{
}

PropertiesPane::~PropertiesPane()
{
}

BEGIN_MESSAGE_MAP(PropertiesPane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()

int PropertiesPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    if (!m_wndList.Create(
            WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_NOCOLUMNHEADER | LVS_SINGLESEL | LVS_NOSORTHEADER,
            rectDummy, this, 1))
    {
        TRACE0("속성 창 목록을 만들지 못했습니다.\n");
        return -1;
    }

    m_wndList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    m_wndList.SetBkColor(COLOR_SURFACE);
    m_wndList.SetTextBkColor(COLOR_SURFACE);

    LVCOLUMN lvc;
    lvc.mask = LVCF_WIDTH | LVCF_FMT;
    lvc.fmt  = LVCFMT_LEFT;
    lvc.cx   = PROP_COL_LABEL_W;
    m_wndList.InsertColumn(0, &lvc);

    lvc.cx = PROP_COL_VALUE_W;
    m_wndList.InsertColumn(1, &lvc);

    return 0;
}

void PropertiesPane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);

    if (m_wndList.GetSafeHwnd() == nullptr)
        return;

    m_wndList.SetWindowPos(nullptr, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);

    int nCol0   = m_wndList.GetColumnWidth(0);
    int nRemain = cx - nCol0 - ::GetSystemMetrics(SM_CXVSCROLL) - PROP_SCROLLBAR_ADJ;
    if (nRemain > 0)
        m_wndList.SetColumnWidth(1, nRemain);
}

void PropertiesPane::SetFileInfo(const CString& strFilePath, const TabularData& data)
{
    m_strFilePath = strFilePath;
    m_arrSheetNames.clear();

    int nDot = strFilePath.ReverseFind(_T('.'));
    if (nDot >= 0) {
        CString strExt = strFilePath.Mid(nDot + 1);
        strExt.MakeLower();
        if (strExt == _T("xlsx") || strExt == _T("xls"))
            m_strFormat = _T("Excel");
        else if (strExt == _T("csv"))
            m_strFormat = _T("CSV");
        else
            m_strFormat = strExt;
    }

    if (data.GetSheetCount() > 0) {
        const DataSheet& sheet = data.GetSheet(0);
        m_nRows = sheet.GetRowCount();
        m_nCols = sheet.GetColumnCount();
    } else {
        m_nRows = 0;
        m_nCols = 0;
    }

    for (int i = 0; i < data.GetSheetCount(); i++)
        m_arrSheetNames.push_back(data.GetSheet(i).m_strName);

    RebuildList();
}

void PropertiesPane::ClearInfo()
{
    m_strFilePath.Empty();
    m_strFormat.Empty();
    m_nRows = 0;
    m_nCols = 0;
    m_arrSheetNames.clear();
    RebuildList();
}

void PropertiesPane::AddSectionRow(LPCTSTR pszLabel)
{
    LVITEM lvi   = {};
    lvi.mask     = LVIF_TEXT | LVIF_PARAM;
    lvi.iItem    = m_wndList.GetItemCount();
    lvi.iSubItem = 0;
    lvi.pszText  = const_cast<LPTSTR>(pszLabel);
    lvi.lParam   = (LPARAM)PROP_ROW_SECTION;
    m_wndList.InsertItem(&lvi);
}

void PropertiesPane::AddDataRow(LPCTSTR pszLabel, LPCTSTR pszValue)
{
    LVITEM lvi   = {};
    lvi.mask     = LVIF_TEXT | LVIF_PARAM;
    lvi.iItem    = m_wndList.GetItemCount();
    lvi.iSubItem = 0;
    lvi.pszText  = const_cast<LPTSTR>(pszLabel);
    lvi.lParam   = (LPARAM)PROP_ROW_DATA;
    int nIdx     = m_wndList.InsertItem(&lvi);
    m_wndList.SetItemText(nIdx, 1, pszValue);
}

void PropertiesPane::RebuildList()
{
    if (m_wndList.GetSafeHwnd() == nullptr)
        return;

    m_wndList.DeleteAllItems();

    if (m_strFilePath.IsEmpty())
        return;

    // 파일명 추출
    CString strName = m_strFilePath;
    int nSlash = max(strName.ReverseFind(_T('\\')), strName.ReverseFind(_T('/')));
    if (nSlash >= 0)
        strName = strName.Mid(nSlash + 1);

    // FILE 섹션
    CString sSecFile, sLabelName, sLabelFmt;
    sSecFile.LoadString(IDS_PROP_SECTION_FILE);
    sLabelName.LoadString(IDS_PROP_LABEL_FILENAME);
    sLabelFmt.LoadString(IDS_PROP_LABEL_FORMAT);
    AddSectionRow(sSecFile);
    AddDataRow(sLabelName, strName);
    AddDataRow(sLabelFmt,  m_strFormat);

    // DATA 섹션
    CString sSecData, sLabelRows, sLabelCols, strRows, strCols;
    sSecData.LoadString(IDS_PROP_SECTION_DATA);
    sLabelRows.LoadString(IDS_PROP_LABEL_ROWS);
    sLabelCols.LoadString(IDS_PROP_LABEL_COLS);
    strRows.Format(_T("%d"), m_nRows);
    strCols.Format(_T("%d"), m_nCols);
    AddSectionRow(sSecData);
    AddDataRow(sLabelRows, strRows);
    AddDataRow(sLabelCols, strCols);

    // SHEETS 섹션 (시트가 2개 이상이거나 Excel 포맷일 때)
    if (!m_arrSheetNames.empty() && (m_arrSheetNames.size() > 1 || m_strFormat == _T("Excel"))) {
        CString sSecSheets, sLabelSheetCount, sLabelSheetN;
        sSecSheets.LoadString(IDS_PROP_SECTION_SHEETS);
        sLabelSheetCount.LoadString(IDS_PROP_LABEL_SHEET_COUNT);
        sLabelSheetN.LoadString(IDS_PROP_LABEL_SHEET_N);
        CString strCount;
        strCount.Format(_T("%d"), (int)m_arrSheetNames.size());
        AddSectionRow(sSecSheets);
        AddDataRow(sLabelSheetCount, strCount);
        for (int i = 0; i < (int)m_arrSheetNames.size(); i++) {
            CString strLabel;
            strLabel.Format((LPCTSTR)sLabelSheetN, i + 1);
            AddDataRow(strLabel, m_arrSheetNames[i]);
        }
    }
}

BOOL PropertiesPane::OnEraseBkgnd(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);
    pDC->FillSolidRect(&rect, COLOR_SURFACE);
    return TRUE;
}

void PropertiesPane::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLVCUSTOMDRAW* pNMCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
    *pResult = CDRF_DODEFAULT;

    switch (pNMCD->nmcd.dwDrawStage) {
    case CDDS_PREPAINT:
        *pResult = CDRF_NOTIFYITEMDRAW;
        break;

    case CDDS_ITEMPREPAINT: {
        PropRowType type = (PropRowType)pNMCD->nmcd.lItemlParam;
        if (type == PROP_ROW_SECTION) {
            pNMCD->clrText   = COLOR_TEXT_DIM;
            pNMCD->clrTextBk = COLOR_SURFACE_ALT;
        } else {
            pNMCD->clrText   = COLOR_TEXT_MID;
            pNMCD->clrTextBk = COLOR_SURFACE;
        }
        *pResult = CDRF_NEWFONT | CDRF_NOTIFYSUBITEMDRAW;
        break;
    }

    case CDDS_ITEMPREPAINT | CDDS_SUBITEM: {
        PropRowType type = (PropRowType)pNMCD->nmcd.lItemlParam;
        if (type == PROP_ROW_SECTION) {
            pNMCD->clrText   = COLOR_TEXT_DIM;
            pNMCD->clrTextBk = COLOR_SURFACE_ALT;
        } else if (pNMCD->iSubItem == 1) {
            pNMCD->clrText   = COLOR_TEXT;
            pNMCD->clrTextBk = COLOR_SURFACE;
        } else {
            pNMCD->clrText   = COLOR_TEXT_DIM;
            pNMCD->clrTextBk = COLOR_SURFACE;
        }
        *pResult = CDRF_NEWFONT;
        break;
    }
    }
}
