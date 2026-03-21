
#pragma once

#include "IExecutionStep.h"
#include "IActionHandler.h"

// ============================================================
// ActionStep — IActionHandler 를 IExecutionStep 으로 래핑 (Phase 5-C)
// 코어 4단계(Input→Mapping→Validation→Output) 이후
// PipelineRunner::Run(ctx, arrExtra)를 통해 동적으로 추가된다.
// ============================================================
class ActionStep : public IExecutionStep
{
public:
    explicit ActionStep(IActionHandler& handler);

    CString GetName() const override;
    BOOL    Execute(ExecutionContext& ctx) override;

private:
    IActionHandler& m_handler;
};
