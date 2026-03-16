
#pragma once

#include <vector>
#include "../common/EnumDefine.h"

// ============================================================
// ValidationIssue — 검증 오류/경고 단건
// ============================================================
class ValidationIssue
{
public:
    ValidationIssue()
        : m_nRow(-1)
        , m_eSeverity(VALIDATION_SEVERITY_ERROR)
    {}

    ValidationIssue(int nRow, const CString& strColumn,
                    const CString& strMsg,
                    ValidationSeverity eSeverity = VALIDATION_SEVERITY_ERROR)
        : m_nRow(nRow)
        , m_strColumn(strColumn)
        , m_strMessage(strMsg)
        , m_eSeverity(eSeverity)
    {}

    int                GetRow()      const { return m_nRow; }
    CString            GetColumn()   const { return m_strColumn; }
    CString            GetMessage()  const { return m_strMessage; }
    ValidationSeverity GetSeverity() const { return m_eSeverity; }

    BOOL IsError()   const { return m_eSeverity == VALIDATION_SEVERITY_ERROR; }
    BOOL IsWarning() const { return m_eSeverity == VALIDATION_SEVERITY_WARNING; }

private:
    int                m_nRow;
    CString            m_strColumn;
    CString            m_strMessage;
    ValidationSeverity m_eSeverity;
};

// ============================================================
// ValidationResult — 전체 검증 결과
// ============================================================
class ValidationResult
{
public:
    void AddIssue(const ValidationIssue& issue) {
        m_arrIssues.push_back(issue);
    }

    void Clear() {
        m_arrIssues.clear();
    }

    BOOL HasErrors() const {
        for (int i = 0; i < (int)m_arrIssues.size(); i++) {
            if (m_arrIssues[i].IsError())
                return TRUE;
        }
        return FALSE;
    }

    BOOL HasWarnings() const {
        for (int i = 0; i < (int)m_arrIssues.size(); i++) {
            if (m_arrIssues[i].IsWarning())
                return TRUE;
        }
        return FALSE;
    }

    int GetIssueCount() const {
        return (int)m_arrIssues.size();
    }

    int GetErrorCount() const {
        int nCount = 0;
        for (int i = 0; i < (int)m_arrIssues.size(); i++) {
            if (m_arrIssues[i].IsError())
                nCount++;
        }
        return nCount;
    }

    int GetWarningCount() const {
        int nCount = 0;
        for (int i = 0; i < (int)m_arrIssues.size(); i++) {
            if (m_arrIssues[i].IsWarning())
                nCount++;
        }
        return nCount;
    }

    const ValidationIssue& GetIssue(int nIndex) const {
        return m_arrIssues[nIndex];
    }

    const std::vector<ValidationIssue>& GetIssues() const {
        return m_arrIssues;
    }

private:
    std::vector<ValidationIssue> m_arrIssues;
};
