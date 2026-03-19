
#pragma once

#include "IExecutionStep.h"
#include "IActionHandler.h"

// ============================================================
// ActionStep — IActionHandler 를 IExecutionStep 으로 래핑
// 파이프라인 코어 4단계 이후 추가 단계로 동적 삽입된다.
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
