
#include "pch.h"
#include "framework.h"
#include "NavigatorPane.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
END_MESSAGE_MAP()

int CNavigatorPane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    CRect rectDummy;
    rectDummy.SetRectEmpty();

    if (!m_wndTree.Create(
            WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
            rectDummy, this, 1))
    {
        TRACE0("네비게이터 트리를 만들지 못했습니다.\n");
        return -1;
    }

    // Phase 1: Input 항목만 표시
    HTREEITEM hInput = m_wndTree.InsertItem(_T("Input"), TVI_ROOT, TVI_LAST);
    m_wndTree.InsertItem(_T("File"), hInput, TVI_LAST);
    m_wndTree.Expand(hInput, TVE_EXPAND);

    return 0;
}

void CNavigatorPane::OnSize(UINT nType, int cx, int cy)
{
    CDockablePane::OnSize(nType, cx, cy);

    if (m_wndTree.GetSafeHwnd() != nullptr)
        m_wndTree.SetWindowPos(nullptr, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
}
