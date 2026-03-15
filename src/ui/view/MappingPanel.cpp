
#include "pch.h"
#include "framework.h"
#include "MappingPanel.h"
#include "Define.h"
#include "Resource.h"

BEGIN_MESSAGE_MAP(MappingPanel, CWnd)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

MappingPanel::MappingPanel()
{
}

MappingPanel::~MappingPanel()
{
}

BOOL MappingPanel::Create(CWnd* pParent, UINT nID)
{
    CRect rectDummy;
    rectDummy.SetRectEmpty();
    return CWnd::Create(nullptr, nullptr,
        WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        rectDummy, pParent, nID);
}

BOOL MappingPanel::OnEraseBkgnd(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);
    pDC->FillSolidRect(&rect, COLOR_WHITE);
    return TRUE;
}

void MappingPanel::OnPaint()
{
    CPaintDC dc(this);

    CRect rect;
    GetClientRect(&rect);
    dc.FillSolidRect(&rect, COLOR_WHITE);

    CString strTitle;
    strTitle.LoadString(IDS_VIEW_MAPPING_TITLE);

    CFont* pOldFont = (CFont*)dc.SelectStockObject(DEFAULT_GUI_FONT);
    dc.SetTextColor(COLOR_TEXT_GHOST);
    dc.SetBkMode(TRANSPARENT);
    dc.DrawText(strTitle, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    dc.SelectObject(pOldFont);
}
