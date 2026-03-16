
#pragma once

#include "ValidationRule.h"
#include "ValidationResult.h"
#include "TabularData.h"
#include <vector>

// ============================================================
// ValidationPanel — Center View 검증 모드 패널 (Phase 3)
// 검증 규칙을 관리하고 데이터 검증 결과를 표시한다.
// ============================================================
class ValidationPanel : public CWnd
{
public:
    ValidationPanel();
    virtual ~ValidationPanel();

    BOOL Create(CWnd* pParent, UINT nID);

    void SetSourceColumns(const std::vector<CString>& arrColumns);
    void SetDataSheet(const DataSheet* pSheet);
    const std::vector<ValidationRule>& GetValidationRules() const { return m_arrRules; }
    void LoadRules(const std::vector<ValidationRule>& arrRules);
    void ClearAll();

private:
    CComboBox  m_cmbColumn;
    CComboBox  m_cmbType;
    CComboBox  m_cmbSeverity;
    CEdit      m_edtPattern;
    CButton    m_btnAdd;
    CButton    m_btnDelete;
    CButton    m_btnClearRules;
    CListCtrl  m_lstRules;
    CButton    m_btnRun;
    CListCtrl  m_lstResults;

    std::vector<CString>        m_arrSourceColumns;
    std::vector<ValidationRule> m_arrRules;
    ValidationResult            m_lastResult;
    BOOL                        m_bHasResult;
    const DataSheet*            m_pDataSheet;

    void CreateControls();
    void UpdateLayout(int cx, int cy);
    void RebuildRulesList();
    void RebuildResultsList();
    void UpdateButtonStates();
    void AddRule();
    void DeleteSelectedRule();
    void RunValidation();

    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnPaint();
    afx_msg void OnBnClickedAdd();
    afx_msg void OnBnClickedDelete();
    afx_msg void OnBnClickedClearRules();
    afx_msg void OnBnClickedRun();
    afx_msg void OnLvnItemChangedRules(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnCustomDrawResults(NMHDR* pNMHDR, LRESULT* pResult);
    DECLARE_MESSAGE_MAP()
};
