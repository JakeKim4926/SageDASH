
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
        , m_pCancelFlag(nullptr)
    {}

    // 취소 플래그 (BatchRunner에서 주입, nullptr이면 취소 체크 없음)
    volatile LONG*                m_pCancelFlag;

    BOOL IsCancelled() const {
        if (!m_pCancelFlag) return FALSE;
        return InterlockedCompareExchange(
            const_cast<volatile LONG*>(m_pCancelFlag), 0, 0) != 0;
    }

    // 입력
    const DataSheet*              m_pSourceSheet;

    // 규칙
    std::vector<MappingRule>      m_arrMappingRules;
    std::vector<ValidationRule>   m_arrValidationRules;

    // 출력 설정
    CString                       m_strOutputPath;

    // MappingStep이 생성한 변환 결과 시트 (비어있으면 원본 사용)
    DataSheet                     m_transformedSheet;

    // 실행 결과
    ValidationResult              m_validationResult;
    CString                       m_strLog;

    BOOL IsValid() const { return m_pSourceSheet != nullptr; }

    // 유효한 출력 대상 시트 반환 — 매핑 결과가 있으면 그것, 없으면 원본
    const DataSheet& GetEffectiveSheet() const {
        return m_transformedSheet.GetRowCount() > 0 ? m_transformedSheet : *m_pSourceSheet;
    }
};
