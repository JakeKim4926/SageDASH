
#pragma once

#include "TabularData.h"
#include "EnumDefine.h"
#include "Resource.h"

class CPropertiesPane : public CDockablePane
{
public:
    CPropertiesPane() noexcept;
    virtual ~CPropertiesPane();

    void SetFileInfo(const CString& strFilePath, const TabularData& data);
    void ClearInfo();

protected:
    CListCtrl m_wndList;

    void RebuildList();

private:
    CString              m_strFilePath;
    CString              m_strFormat;
    int                  m_nRows    = 0;
    int                  m_nCols    = 0;
    std::vector<CString> m_arrSheetNames;

    void AddSectionRow(LPCTSTR pszLabel);
    void AddDataRow(LPCTSTR pszLabel, LPCTSTR pszValue);

    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()
};
