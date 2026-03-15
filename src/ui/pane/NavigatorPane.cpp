
#include "pch.h"
#include "framework.h"
#include "NavigatorPane.h"
#include "Define.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

CNavigatorPane::CNavigatorPane() noexcept
{
}

CNavigatorPane::~CNavigatorPane()
{
}

BEGIN_MESSAGE_MAP(CNavigatorPane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
END_MESSAGE_MAP()

int CNavigatorPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    if (!m_wndTree.Create(
            WS_CHILD | WS_VISIBLE | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
            rectDummy, this, 1))
    {
        TRACE0("네비게이터 트리를 만들지 못했습니다.\n");
        return -1;
    }

    // INPUT 섹션
    HTREEITEM hInput = InsertNavItem(_T("INPUT"), TVI_ROOT, NAV_ITEM_SECTION);
    InsertNavItem(_T("File"), hInput, NAV_ITEM_ACTIVE);
    m_wndTree.Expand(hInput, TVE_EXPAND);

    // PIPELINE 섹션
    HTREEITEM hPipeline = InsertNavItem(_T("PIPELINE"), TVI_ROOT, NAV_ITEM_SECTION);
    InsertNavItem(_T("Preview"),    hPipeline, NAV_ITEM_DISABLED);
    InsertNavItem(_T("Mapping"),    hPipeline, NAV_ITEM_DISABLED);
    InsertNavItem(_T("Validation"), hPipeline, NAV_ITEM_DISABLED);
    InsertNavItem(_T("Output"),     hPipeline, NAV_ITEM_DISABLED);
    m_wndTree.Expand(hPipeline, TVE_EXPAND);

    // ACTIONS 섹션
    HTREEITEM hActions = InsertNavItem(_T("ACTIONS"), TVI_ROOT, NAV_ITEM_SECTION);
    InsertNavItem(_T("Export"),   hActions, NAV_ITEM_DISABLED);
    InsertNavItem(_T("Schedule"), hActions, NAV_ITEM_DISABLED);
    m_wndTree.Expand(hActions, TVE_EXPAND);

    return 0;
}

HTREEITEM CNavigatorPane::InsertNavItem(LPCTSTR pszLabel, HTREEITEM hParent, NavItemType type)
{
    TVINSERTSTRUCT tvis        = {};
    tvis.hParent               = hParent;
    tvis.hInsertAfter          = TVI_LAST;
    tvis.item.mask             = TVIF_TEXT | TVIF_PARAM;
    tvis.item.pszText          = const_cast<LPTSTR>(pszLabel);
    tvis.item.lParam           = (LPARAM)type;
    return m_wndTree.InsertItem(&tvis);
}

void CNavigatorPane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);

    if (m_wndTree.GetSafeHwnd() != nullptr)
        m_wndTree.SetWindowPos(nullptr, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
}

void CNavigatorPane::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMTVCUSTOMDRAW* pNMCD = reinterpret_cast<NMTVCUSTOMDRAW*>(pNMHDR);
    *pResult = CDRF_DODEFAULT;

    switch (pNMCD->nmcd.dwDrawStage) {
    case CDDS_PREPAINT:
        *pResult = CDRF_NOTIFYITEMDRAW;
        break;

    case CDDS_ITEMPREPAINT: {
        NavItemType type = (NavItemType)pNMCD->nmcd.lItemlParam;
        switch (type) {
        case NAV_ITEM_SECTION:
            pNMCD->clrText   = COLOR_TEXT_DIM;
            pNMCD->clrTextBk = COLOR_SURFACE;
            break;
        case NAV_ITEM_ACTIVE:
            pNMCD->clrText   = COLOR_ACCENT_DARK;
            pNMCD->clrTextBk = COLOR_ACCENT_LIGHT;
            break;
        case NAV_ITEM_DISABLED:
            pNMCD->clrText   = COLOR_TEXT_MUTED;
            pNMCD->clrTextBk = COLOR_SURFACE;
            break;
        }
        *pResult = CDRF_NEWFONT;
        break;
    }
    }
}
