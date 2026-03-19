
#include "pch.h"
#include "framework.h"
#include "PipelineRunner.h"
#include "pipeline/InputStep.h"
#include "pipeline/MappingStep.h"
#include "pipeline/ValidationStep.h"
#include "pipeline/OutputStep.h"

CString PipelineRunner::Run(ExecutionContext& ctx)
{
    ctx.m_strLog.Empty();

    InputStep      stepInput;
    MappingStep    stepMapping;
    ValidationStep stepValidation;
    OutputStep     stepOutput;

    IExecutionStep* arrSteps[] = {
        &stepInput,
        &stepMapping,
        &stepValidation,
        &stepOutput
    };
    constexpr int STEP_COUNT = 4;

    for (int i = 0; i < STEP_COUNT; i++) {
        IExecutionStep* pStep = arrSteps[i];
        CString strStepLog;
        strStepLog.Format(_T("▶ %s\r\n"), (LPCTSTR)pStep->GetName());
        ctx.m_strLog += strStepLog;

        BOOL bOk = pStep->Execute(ctx);
        if (!bOk) {
            CString strErr;
            strErr.Format(_T("[파이프라인] '%s' 단계에서 중단되었습니다."), (LPCTSTR)pStep->GetName());
            return strErr;
        }
    }

    return CString();
}
