
#include "pch.h"
#include "framework.h"
#include "BatchRunner.h"
#include "WorkbookService.h"
#include "PipelineRunner.h"
#include "ExecutionContext.h"
#include "SageException.h"
#include "Define.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ============================================================
// 워커 스레드에 전달되는 파라미터 구조체
// ============================================================
struct BatchThreadParam
{
    BatchRunner* pRunner;
};

BatchRunner::BatchRunner()
    : m_hNotify(nullptr)
    , m_nCancelFlag(0)
    , m_nRunningFlag(0)
{
}

BOOL BatchRunner::IsRunning() const
{
    return InterlockedCompareExchange(
        const_cast<volatile LONG*>(&m_nRunningFlag), 0, 0) != 0;
}

void BatchRunner::RequestCancel()
{
    InterlockedExchange(&m_nCancelFlag, 1);
}

void BatchRunner::Start(const std::vector<BatchJob>& arrJobs, HWND hNotify)
{
    if (IsRunning())
        return;

    m_arrJobs  = arrJobs;
    m_hNotify  = hNotify;
    InterlockedExchange(&m_nCancelFlag, 0);
    InterlockedExchange(&m_nRunningFlag, 1);

    BatchThreadParam* pParam = new BatchThreadParam;
    pParam->pRunner = this;

    AfxBeginThread(RunThread, pParam, THREAD_PRIORITY_BELOW_NORMAL);
}

UINT __cdecl BatchRunner::RunThread(LPVOID pParam)
{
    BatchThreadParam* pData = reinterpret_cast<BatchThreadParam*>(pParam);
    pData->pRunner->RunInternal();
    delete pData;
    return 0;
}

void BatchRunner::RunInternal()
{
    BatchSummary* pSummary = new BatchSummary;
    WorkbookService svcWorkbook;
    PipelineRunner  runner;

    int nTotal = (int)m_arrJobs.size();

    for (int i = 0; i < nTotal; i++) {
        if (InterlockedCompareExchange(&m_nCancelFlag, 0, 0) != 0) {
            pSummary->SetCancelled(TRUE);
            break;
        }

        const BatchJob& job = m_arrJobs[i];

        BatchResult* pResult = new BatchResult;
        pResult->SetInputPath(job.GetInputPath());

        try {
            TabularData data;
            svcWorkbook.LoadFromFile(job.GetInputPath(), data);

            if (data.GetSheetCount() == 0 || data.GetSheet(0).GetRowCount() == 0) {
                CString strErr;
                strErr.Format(_T("입력 파일에 데이터가 없습니다: %s"),
                              (LPCTSTR)job.GetInputPath());
                throw SageException(strErr);
            }

            ExecutionContext ctx;
            ctx.m_pSourceSheet      = &data.GetSheet(0);
            ctx.m_arrMappingRules   = job.GetProject().m_arrMappingRules;
            ctx.m_arrValidationRules = job.GetProject().m_arrValidationRules;
            ctx.m_strOutputPath     = job.GetProject().m_strOutputPath;

            CString strError = runner.Run(ctx);
            if (!strError.IsEmpty()) {
                throw SageException(strError);
            }

            pResult->SetSuccess(TRUE);
            pResult->SetLog(ctx.m_strLog);

        } catch (SageException& e) {
            pResult->SetSuccess(FALSE);
            pResult->SetErrorMessage(e.GetMessage());
        } catch (...) {
            pResult->SetSuccess(FALSE);
            pResult->SetErrorMessage(_T("알 수 없는 오류가 발생했습니다."));
        }

        pSummary->AddResult(*pResult);

        // WM_BATCH_JOB_DONE: 수신측이 pResult를 delete해야 한다
        if (::IsWindow(m_hNotify)) {
            ::PostMessage(m_hNotify, WM_BATCH_JOB_DONE,
                          (WPARAM)i, (LPARAM)pResult);
        } else {
            delete pResult;
        }

        // WM_BATCH_PROGRESS
        if (::IsWindow(m_hNotify)) {
            ::PostMessage(m_hNotify, WM_BATCH_PROGRESS,
                          (WPARAM)(i + 1), (LPARAM)nTotal);
        }
    }

    InterlockedExchange(&m_nRunningFlag, 0);

    // WM_BATCH_COMPLETE: 수신측이 pSummary를 delete해야 한다
    if (::IsWindow(m_hNotify)) {
        ::PostMessage(m_hNotify, WM_BATCH_COMPLETE, 0, (LPARAM)pSummary);
    } else {
        delete pSummary;
    }
}
