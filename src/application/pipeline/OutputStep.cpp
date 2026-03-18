
#include "pch.h"
#include "framework.h"
#include "OutputStep.h"

CString OutputStep::GetName() const
{
    return _T("결과 출력");
}

BOOL OutputStep::Execute(ExecutionContext& ctx)
{
    if (ctx.m_strOutputPath.IsEmpty()) {
        ctx.m_strLog += _T("[출력] 경로 없음 — 건너뜀\r\n");
        return TRUE;
    }

    // GetEffectiveSheet() 를 TabularData 로 래핑해서 ExportService 에 전달
    TabularData outData;
    outData.m_strFilePath = ctx.m_strOutputPath;
    outData.m_arrSheets.push_back(ctx.GetEffectiveSheet());

    ExportService svc;
    CString strError = svc.Export(outData, ctx.m_strOutputPath);
    if (!strError.IsEmpty()) {
        CString strMsg;
        strMsg.Format(_T("[출력] 오류: %s\r\n"), (LPCTSTR)strError);
        ctx.m_strLog += strMsg;
        return FALSE;
    }

    CString strMsg;
    strMsg.Format(_T("[출력] 저장 완료 — %s\r\n"), (LPCTSTR)ctx.m_strOutputPath);
    ctx.m_strLog += strMsg;
    return TRUE;
}
