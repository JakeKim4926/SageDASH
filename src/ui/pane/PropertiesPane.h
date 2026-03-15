
#pragma once

class CPropertiesPane : public CDockablePane
{
public:
    CPropertiesPane() noexcept;
    virtual ~CPropertiesPane();

    void SetFileInfo(const CString& strFileName, int nRows, int nCols);
    void ClearInfo();

protected:
    CListCtrl m_wndList;

    void RebuildList();

private:
    CString m_strFileName;
    int     m_nRows = 0;
    int     m_nCols = 0;

    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()
};
