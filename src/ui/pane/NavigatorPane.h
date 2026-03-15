
#pragma once

class CNavigatorPane : public CDockablePane
{
public:
    CNavigatorPane() noexcept;
    virtual ~CNavigatorPane();

protected:
    CTreeCtrl m_wndTree;

    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()
};
