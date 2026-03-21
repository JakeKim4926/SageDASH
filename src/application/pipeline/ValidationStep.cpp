
#include "pch.h"
#include "framework.h"
#include "ValidationStep.h"

CString ValidationStep::GetName() const
{
    return _T("데이터 검증");
}

BOOL ValidationStep::Execute(ExecutionContext& ctx)
{
    if (ctx.IsCancelled()) {
        ctx.m_strLog += _T("[검증] 취소됨\r\n");
        return FALSE;
    }

    if (ctx.m_arrValidationRules.empty()) {
        ctx.m_strLog += _T("[검증] 규칙 없음 — 건너뜀\r\n");
        return TRUE;
    }

    ValidationService svc;
    ctx.m_validationResult = svc.Validate(ctx.GetEffectiveSheet(), ctx.m_arrValidationRules);

    int nErrors   = ctx.m_validationResult.GetErrorCount();
    int nWarnings = ctx.m_validationResult.GetWarningCount();

    CString strMsg;
    strMsg.Format(_T("[검증] 완료 — 오류 %d건, 경고 %d건\r\n"), nErrors, nWarnings);
    ctx.m_strLog += strMsg;

    return (nErrors == 0) ? TRUE : FALSE;
}
