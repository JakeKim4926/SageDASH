
#pragma once

class OutputWnd : public CDockablePane
{
public:
    OutputWnd() noexcept;
    virtual ~OutputWnd();

    void UpdateFonts();
    void AppendLog(const CString& strMessage);

protected:
    CRichEditCtrl m_wndRich;
    CFont         m_fontLog;

    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()

private:
    void TrimLogIfNeeded();
};
