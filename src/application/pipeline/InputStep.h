
#pragma once

#include "IExecutionStep.h"

// ============================================================
// InputStep — 파이프라인 1단계: 입력 데이터 유효성 확인
// m_pSourceSheet 가 존재하고 데이터 행이 있는지 검증한다.
// ============================================================
class InputStep : public IExecutionStep
{
public:
    CString GetName() const override;
    BOOL    Execute(ExecutionContext& ctx) override;
};
