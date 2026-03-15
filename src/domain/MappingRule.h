
#pragma once

// ============================================================
// MappingRule — Source 컬럼 → Target 컬럼 매핑 규칙
// ============================================================
class MappingRule
{
public:
    MappingRule() : m_isRequired(FALSE) {}

    MappingRule(const CString& strSource, const CString& strTarget, BOOL isRequired = FALSE)
        : m_strSourceColumn(strSource)
        , m_strTargetColumn(strTarget)
        , m_isRequired(isRequired)
    {}

    CString GetSourceColumn() const { return m_strSourceColumn; }
    CString GetTargetColumn() const { return m_strTargetColumn; }
    BOOL    IsRequired()      const { return m_isRequired; }

    void SetSourceColumn(const CString& str) { m_strSourceColumn = str; }
    void SetTargetColumn(const CString& str) { m_strTargetColumn = str; }
    void SetRequired(BOOL isRequired)        { m_isRequired = isRequired; }

    BOOL IsValid() const {
        return !m_strSourceColumn.IsEmpty() && !m_strTargetColumn.IsEmpty();
    }

private:
    CString m_strSourceColumn;
    CString m_strTargetColumn;
    BOOL    m_isRequired;
};
