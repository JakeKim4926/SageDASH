
#pragma once

class COutputWnd : public CDockablePane
{
public:
    COutputWnd() noexcept;
    virtual ~COutputWnd();

    void UpdateFonts();
    void AppendLog(const CString& strMessage);

protected:
    CListBox m_wndLog;

    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()
};
