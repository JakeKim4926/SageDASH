
#pragma once

#include "TabularData.h"
#include "MappingRule.h"
#include "ValidationRule.h"
#include "ValidationResult.h"
#include <vector>

// ============================================================
// ExecutionContext — 자동화 파이프라인 단계 간 공유 컨텍스트
// ============================================================
class ExecutionContext
{
public:
    ExecutionContext()
        : m_pSourceSheet(nullptr)
    {}

    // 입력
    const DataSheet*              m_pSourceSheet;

    // 규칙
    std::vector<MappingRule>      m_arrMappingRules;
    std::vector<ValidationRule>   m_arrValidationRules;

    // 출력 설정
    CString                       m_strOutputPath;

    // 실행 결과
    ValidationResult              m_validationResult;
    CString                       m_strLog;

    BOOL IsValid() const { return m_pSourceSheet != nullptr; }
};
