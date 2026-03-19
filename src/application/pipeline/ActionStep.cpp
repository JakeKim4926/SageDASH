
#include "pch.h"
#include "framework.h"
#include "ActionStep.h"
#include "SageException.h"

ActionStep::ActionStep(IActionHandler& handler)
    : m_handler(handler)
{
}

CString ActionStep::GetName() const
{
    return m_handler.GetName();
}

BOOL ActionStep::Execute(ExecutionContext& ctx)
{
    // GetEffectiveSheet() 는 DataSheet 를 반환 — TabularData 로 래핑하여 핸들러에 전달
    const DataSheet& sheet = ctx.GetEffectiveSheet();

    TabularData data;
    data.m_arrSheets.push_back(sheet);

    try {
        m_handler.Execute(data);
    } catch (SageException& e) {
        CString strErr;
        strErr.Format(_T("[%s] %s"), (LPCTSTR)GetName(), (LPCTSTR)e.GetMessage());
        ctx.m_strLog += strErr + _T("\r\n");
        return FALSE;
    }

    return TRUE;
}
