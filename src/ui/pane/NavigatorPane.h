
#pragma once

#include "EnumDefine.h"

class CNavigatorPane : public CDockablePane
{
public:
    CNavigatorPane() noexcept;
    virtual ~CNavigatorPane();

protected:
    CTreeCtrl m_wndTree;

    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()

private:
    HTREEITEM InsertNavItem(const CString& strLabel, HTREEITEM hParent, NavItemType type);
};
