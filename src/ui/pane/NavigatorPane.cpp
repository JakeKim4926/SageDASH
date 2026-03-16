
#include "pch.h"
#include "framework.h"
#include "NavigatorPane.h"
#include "Define.h"
#include "Resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

NavigatorPane::NavigatorPane() noexcept
{
}

NavigatorPane::~NavigatorPane()
{
}

BEGIN_MESSAGE_MAP(NavigatorPane, CDockablePane)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnCustomDraw)
    ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelChanged)
END_MESSAGE_MAP()

int NavigatorPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
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

    CString str;

    // INPUT 섹션
    str.LoadString(IDS_NAV_SECTION_INPUT);
    HTREEITEM hInput = InsertNavItem(str, TVI_ROOT, NAV_ITEM_SECTION);
    str.LoadString(IDS_NAV_ITEM_FILE);
    InsertNavItem(str, hInput, NAV_ITEM_ACTIVE);
    m_wndTree.Expand(hInput, TVE_EXPAND);

    // PIPELINE 섹션
    str.LoadString(IDS_NAV_SECTION_PIPELINE);
    HTREEITEM hPipeline = InsertNavItem(str, TVI_ROOT, NAV_ITEM_SECTION);
    str.LoadString(IDS_NAV_ITEM_PREVIEW);
    m_hPreview = InsertNavItem(str, hPipeline, NAV_ITEM_DISABLED);
    str.LoadString(IDS_NAV_ITEM_MAPPING);
    m_hMapping = InsertNavItem(str, hPipeline, NAV_ITEM_DISABLED);
    str.LoadString(IDS_NAV_ITEM_VALIDATION);
    m_hValidation = InsertNavItem(str, hPipeline, NAV_ITEM_DISABLED);
    str.LoadString(IDS_NAV_ITEM_OUTPUT);
    InsertNavItem(str, hPipeline, NAV_ITEM_DISABLED);
    m_wndTree.Expand(hPipeline, TVE_EXPAND);

    // ACTIONS 섹션
    str.LoadString(IDS_NAV_SECTION_ACTIONS);
    HTREEITEM hActions = InsertNavItem(str, TVI_ROOT, NAV_ITEM_SECTION);
    str.LoadString(IDS_NAV_ITEM_EXPORT);
    InsertNavItem(str, hActions, NAV_ITEM_DISABLED);
    str.LoadString(IDS_NAV_ITEM_SCHEDULE);
    InsertNavItem(str, hActions, NAV_ITEM_DISABLED);
    m_wndTree.Expand(hActions, TVE_EXPAND);

    return 0;
}

HTREEITEM NavigatorPane::InsertNavItem(const CString& strLabel, HTREEITEM hParent, NavItemType type)
{
    TVINSERTSTRUCT tvis        = {};
    tvis.hParent               = hParent;
    tvis.hInsertAfter          = TVI_LAST;
    tvis.item.mask             = TVIF_TEXT | TVIF_PARAM;
    tvis.item.pszText          = const_cast<LPTSTR>((LPCTSTR)strLabel);
    tvis.item.lParam           = (LPARAM)type;
    return m_wndTree.InsertItem(&tvis);
}

void NavigatorPane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);

    if (m_wndTree.GetSafeHwnd() != nullptr)
        m_wndTree.SetWindowPos(nullptr, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
}

void NavigatorPane::ActivatePipelineItems(BOOL bActive)
{
    if (m_wndTree.GetSafeHwnd() == nullptr)
        return;

    NavItemType type = bActive ? NAV_ITEM_ACTIVE : NAV_ITEM_DISABLED;
    m_wndTree.SetItemData(m_hPreview,    (DWORD_PTR)type);
    m_wndTree.SetItemData(m_hMapping,    (DWORD_PTR)type);
    m_wndTree.SetItemData(m_hValidation, (DWORD_PTR)type);
    m_wndTree.Invalidate();
}

void NavigatorPane::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMTREEVIEW* pNMTV = reinterpret_cast<NMTREEVIEW*>(pNMHDR);
    *pResult = 0;

    HTREEITEM hSel = pNMTV->itemNew.hItem;
    if (hSel == nullptr)
        return;

    // ACTIVE 상태인 항목만 반응한다
    NavItemType type = (NavItemType)m_wndTree.GetItemData(hSel);
    if (type != NAV_ITEM_ACTIVE)
        return;

    CWnd* pMain = AfxGetMainWnd();
    if (pMain == nullptr)
        return;

    if (hSel == m_hPreview) {
        pMain->PostMessage(WM_SWITCH_CENTER_VIEW, (WPARAM)VIEW_MODE_GRID);
    } else if (hSel == m_hMapping) {
        pMain->PostMessage(WM_SWITCH_CENTER_VIEW, (WPARAM)VIEW_MODE_MAPPING);
    } else if (hSel == m_hValidation) {
        pMain->PostMessage(WM_SWITCH_CENTER_VIEW, (WPARAM)VIEW_MODE_VALIDATION);
    }
}

void NavigatorPane::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
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
