
#pragma once

#include "EnumDefine.h"

class NavigatorPane : public CDockablePane
{
public:
    NavigatorPane() noexcept;
    virtual ~NavigatorPane();

    void ActivatePipelineItems(BOOL bActive);

protected:
    CTreeCtrl m_wndTree;

    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()

private:
    HTREEITEM m_hPreview    = nullptr;
    HTREEITEM m_hMapping    = nullptr;
    HTREEITEM m_hValidation = nullptr;

    HTREEITEM InsertNavItem(const CString& strLabel, HTREEITEM hParent, NavItemType type);
};
