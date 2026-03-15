
#include "pch.h"
#include "framework.h"
#include "PropertiesPane.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPropertiesPane::CPropertiesPane() noexcept
{
}

CPropertiesPane::~CPropertiesPane()
{
}

BEGIN_MESSAGE_MAP(CPropertiesPane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
END_MESSAGE_MAP()

int CPropertiesPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

    m_wndList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    LVCOLUMN lvc;
    lvc.mask  = LVCF_WIDTH | LVCF_FMT;
    lvc.fmt   = LVCFMT_LEFT;
    lvc.cx    = 80;
    m_wndList.InsertColumn(0, &lvc);

    lvc.cx = 200;
    m_wndList.InsertColumn(1, &lvc);

    RebuildList();
    return 0;
}

void CPropertiesPane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);

    if (m_wndList.GetSafeHwnd() != nullptr)
    {
        m_wndList.SetWindowPos(nullptr, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);

        // 두 번째 컬럼이 남은 공간을 채우도록 조정
        int nCol0 = m_wndList.GetColumnWidth(0);
        int nRemain = cx - nCol0 - ::GetSystemMetrics(SM_CXVSCROLL) - 2;
        if (nRemain > 0)
            m_wndList.SetColumnWidth(1, nRemain);
    }
}

void CPropertiesPane::SetFileInfo(const CString& strFileName, int nRows, int nCols)
{
    m_strFileName = strFileName;
    m_nRows       = nRows;
    m_nCols       = nCols;
    RebuildList();
}

void CPropertiesPane::ClearInfo()
{
    m_strFileName.Empty();
    m_nRows = 0;
    m_nCols = 0;
    RebuildList();
}

void CPropertiesPane::RebuildList()
{
    if (m_wndList.GetSafeHwnd() == nullptr)
        return;

    m_wndList.DeleteAllItems();

    if (m_strFileName.IsEmpty())
        return;

    // 파일명 (경로에서 파일명만 추출)
    CString strName = m_strFileName;
    int nSlash = max(strName.ReverseFind(_T('\\')), strName.ReverseFind(_T('/')));
    if (nSlash >= 0)
        strName = strName.Mid(nSlash + 1);

    auto addRow = [&](LPCTSTR pszLabel, LPCTSTR pszValue) {
        int nIdx = m_wndList.InsertItem(m_wndList.GetItemCount(), pszLabel);
        m_wndList.SetItemText(nIdx, 1, pszValue);
    };

    CString strRows, strCols;
    strRows.Format(_T("%d"), m_nRows);
    strCols.Format(_T("%d"), m_nCols);

    addRow(_T("파일명"), strName);
    addRow(_T("행 수"),  strRows);
    addRow(_T("열 수"),  strCols);
}
