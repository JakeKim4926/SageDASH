
#pragma once

#include "IExecutionStep.h"
#include "ExportService.h"

// ============================================================
// OutputStep — 파이프라인 4단계: 결과 출력
// ctx.m_strOutputPath 가 비어있으면 건너뛴다.
// GetEffectiveSheet() (매핑 결과 또는 원본)를 ExportService 로 저장한다.
// ============================================================
class OutputStep : public IExecutionStep
{
public:
    CString GetName() const override;
    BOOL    Execute(ExecutionContext& ctx) override;
};
