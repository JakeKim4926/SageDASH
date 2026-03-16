
#pragma once

#include "MappingRule.h"
#include <vector>

// ============================================================
// MappingPanel — Center View 매핑 모드 패널 (Phase 3)
// Source 컬럼 → Target 컬럼 매핑 규칙을 관리한다.
// ============================================================
class MappingPanel : public CWnd
{
public:
    MappingPanel();
    virtual ~MappingPanel();

    BOOL Create(CWnd* pParent, UINT nID);

    void SetSourceColumns(const std::vector<CString>& arrColumns);
    const std::vector<MappingRule>& GetMappingRules() const { return m_arrRules; }
    void ClearAll();

private:
    CComboBox  m_cmbSource;
    CEdit      m_edtTarget;
    CButton    m_chkRequired;
    CButton    m_btnAdd;
    CButton    m_btnDelete;
    CButton    m_btnAutoMap;
    CButton    m_btnClear;
    CListCtrl  m_lstRules;

    std::vector<CString>     m_arrSourceColumns;
    std::vector<MappingRule> m_arrRules;

    void CreateControls();
    void UpdateLayout(int cx, int cy);
    void RebuildList();
    void UpdateButtonStates();
    void UpdateStatus();
    void AddRule();
    void DeleteSelectedRule();
    void AutoMap();

    int     GetUnmappedCount() const;
    CString BuildStatusString() const;

    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnBnClickedAdd();
    afx_msg void OnBnClickedDelete();
    afx_msg void OnBnClickedAutoMap();
    afx_msg void OnBnClickedClear();
    afx_msg void OnLvnItemChangedRules(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnCustomDrawRules(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()
};
