
#pragma once

#include "ExecutionContext.h"
#include "IExecutionStep.h"
#include <vector>

// ============================================================
// PipelineRunner — 자동화 파이프라인 실행 엔진
// InputStep → MappingStep → ValidationStep → OutputStep 순서로
// ExecutionContext 를 공유하며 순차 실행한다.
// 반환값: 성공 시 빈 CString, 실패 시 오류 단계명 + 메시지
//
// Run(ctx)           : 코어 4단계만 실행
// Run(ctx, arrExtra) : 코어 4단계 후 추가 단계(ActionStep 등) 실행
// ============================================================
class PipelineRunner
{
public:
    CString Run(ExecutionContext& ctx);
    CString Run(ExecutionContext& ctx, const std::vector<IExecutionStep*>& arrExtra);
};
