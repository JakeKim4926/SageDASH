
#pragma once

#include "../common/EnumDefine.h"

// ============================================================
// ValidationRule — 컬럼 단위 검증 규칙
// ============================================================
class ValidationRule
{
public:
    ValidationRule()
        : m_eType(VALIDATION_REQUIRED)
        , m_eSeverity(VALIDATION_SEVERITY_ERROR)
    {}

    ValidationRule(const CString& strColumn, ValidationType eType,
                   ValidationSeverity eSeverity = VALIDATION_SEVERITY_ERROR)
        : m_strColumnName(strColumn)
        , m_eType(eType)
        , m_eSeverity(eSeverity)
    {}

    CString            GetColumnName() const { return m_strColumnName; }
    ValidationType     GetType()       const { return m_eType; }
    ValidationSeverity GetSeverity()   const { return m_eSeverity; }
    CString            GetPattern()    const { return m_strPattern; }
    CString            GetErrorMsg()   const { return m_strErrorMsg; }

    void SetColumnName(const CString& str)   { m_strColumnName = str; }
    void SetType(ValidationType eType)        { m_eType = eType; }
    void SetSeverity(ValidationSeverity e)    { m_eSeverity = e; }
    void SetPattern(const CString& str)       { m_strPattern = str; }
    void SetErrorMsg(const CString& str)      { m_strErrorMsg = str; }

private:
    CString            m_strColumnName;
    ValidationType     m_eType;
    ValidationSeverity m_eSeverity;
    CString            m_strPattern;   // VALIDATION_FORMAT 일 때 사용
    CString            m_strErrorMsg;  // 오류 메시지 (LoadString으로 채워 넣는다)
};
