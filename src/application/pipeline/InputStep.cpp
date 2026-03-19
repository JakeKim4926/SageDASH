
#include "pch.h"
#include "framework.h"
#include "InputStep.h"

CString InputStep::GetName() const
{
    return _T("입력 확인");
}

BOOL InputStep::Execute(ExecutionContext& ctx)
{
    if (ctx.m_pSourceSheet == nullptr) {
        ctx.m_strLog += _T("[입력] 오류: 소스 데이터가 없습니다.\r\n");
        return FALSE;
    }

    int nRows = ctx.m_pSourceSheet->GetRowCount();
    if (nRows < 2) {
        ctx.m_strLog += _T("[입력] 오류: 헤더만 있고 데이터 행이 없습니다.\r\n");
        return FALSE;
    }

    int nCols = ctx.m_pSourceSheet->GetColumnCount();
    CString strMsg;
    strMsg.Format(_T("[입력] 확인 완료 — %d행 × %d열\r\n"), nRows - 1, nCols);
    ctx.m_strLog += strMsg;
    return TRUE;
}
