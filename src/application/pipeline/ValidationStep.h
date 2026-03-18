
#pragma once

#include "IExecutionStep.h"
#include "ValidationService.h"

// ============================================================
// ValidationStep — 파이프라인 3단계: 데이터 검증
// ValidationService 를 사용해 검증 규칙을 적용하고
// ctx.m_validationResult 에 결과를 저장한다.
// ERROR 심각도 이슈가 있으면 FALSE 반환.
// ============================================================
class ValidationStep : public IExecutionStep
{
public:
    CString GetName() const override;
    BOOL    Execute(ExecutionContext& ctx) override;
};
