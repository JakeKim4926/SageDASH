
#include "pch.h"
#include "framework.h"
#include "PipelineRunner.h"
#include "pipeline/InputStep.h"
#include "pipeline/MappingStep.h"
#include "pipeline/ValidationStep.h"
#include "pipeline/OutputStep.h"

static CString RunSteps(ExecutionContext& ctx, IExecutionStep** arrSteps, int nCount)
{
    for (int i = 0; i < nCount; i++) {
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

CString PipelineRunner::Run(ExecutionContext& ctx)
{
    ctx.m_strLog.Empty();

    InputStep      stepInput;
    MappingStep    stepMapping;
    ValidationStep stepValidation;
    OutputStep     stepOutput;

    IExecutionStep* arrCore[] = {
        &stepInput,
        &stepMapping,
        &stepValidation,
        &stepOutput
    };

    return RunSteps(ctx, arrCore, 4);
}

CString PipelineRunner::Run(ExecutionContext& ctx, const std::vector<IExecutionStep*>& arrExtra)
{
    ctx.m_strLog.Empty();

    InputStep      stepInput;
    MappingStep    stepMapping;
    ValidationStep stepValidation;
    OutputStep     stepOutput;

    IExecutionStep* arrCore[] = {
        &stepInput,
        &stepMapping,
        &stepValidation,
        &stepOutput
    };

    CString strErr = RunSteps(ctx, arrCore, 4);
    if (!strErr.IsEmpty())
        return strErr;

    for (int i = 0; i < (int)arrExtra.size(); i++) {
        IExecutionStep* pStep = arrExtra[i];
        CString strStepLog;
        strStepLog.Format(_T("▶ %s\r\n"), (LPCTSTR)pStep->GetName());
        ctx.m_strLog += strStepLog;

        BOOL bOk = pStep->Execute(ctx);
        if (!bOk) {
            CString strFail;
            strFail.Format(_T("[파이프라인] '%s' 단계에서 중단되었습니다."), (LPCTSTR)pStep->GetName());
            return strFail;
        }
    }

    return CString();
}
