
#include "pch.h"
#include "framework.h"
#include "BatchRunner.h"
#include "WorkbookService.h"
#include "PipelineRunner.h"
#include "ExecutionContext.h"
#include "SageException.h"
#include "SageMgr.h"
#include "Define.h"
#include "Resource.h"

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
    , m_hThreadHandle(NULL)
{
}

BatchRunner::~BatchRunner()
{
    if (IsRunning()) {
        RequestCancel();
        if (m_hThreadHandle != NULL)
            ::WaitForSingleObject(m_hThreadHandle, BATCH_SHUTDOWN_TIMEOUT_MS);
    }
    if (m_hThreadHandle != NULL) {
        ::CloseHandle(m_hThreadHandle);
        m_hThreadHandle = NULL;
    }
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

    if (m_hThreadHandle != NULL) {
        ::CloseHandle(m_hThreadHandle);
        m_hThreadHandle = NULL;
    }

    BatchThreadParam* pParam = new BatchThreadParam;
    pParam->pRunner = this;

    CWinThread* pThread = AfxBeginThread(RunThread, pParam, THREAD_PRIORITY_BELOW_NORMAL);
    if (pThread != nullptr) {
        ::DuplicateHandle(::GetCurrentProcess(), pThread->m_hThread,
                          ::GetCurrentProcess(), &m_hThreadHandle,
                          0, FALSE, DUPLICATE_SAME_ACCESS);
    }
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

        CString strJobStart;
        strJobStart.Format(_T("[Batch] Job %d/%d 시작: %s"),
                           i + 1, nTotal, (LPCTSTR)job.GetInputPath());
        sageMgr.Log(strJobStart);

        BatchResult* pResult = new BatchResult;
        pResult->SetInputPath(job.GetInputPath());

        try {
            TabularData data;
            svcWorkbook.LoadFromFile(job.GetInputPath(), data);

            if (data.GetSheetCount() == 0 || data.GetSheet(0).GetRowCount() == 0) {
                CString strFmt;
                strFmt.LoadString(IDS_BATCH_ERR_EMPTY_DATA);
                CString strErr;
                strErr.Format(strFmt, (LPCTSTR)job.GetInputPath());
                throw SageException(strErr);
            }

            ExecutionContext ctx;
            ctx.m_pCancelFlag       = &m_nCancelFlag;
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
            CString strErr;
            strErr.LoadString(IDS_BATCH_ERR_UNKNOWN);
            pResult->SetSuccess(FALSE);
            pResult->SetErrorMessage(strErr);
        }

        CString strJobDone;
        if (pResult->IsSuccess()) {
            strJobDone.Format(_T("[Batch] Job %d/%d 완료: %s"),
                              i + 1, nTotal, (LPCTSTR)job.GetInputPath());
        } else {
            strJobDone.Format(_T("[Batch] Job %d/%d 실패: %s — %s"),
                              i + 1, nTotal, (LPCTSTR)job.GetInputPath(),
                              (LPCTSTR)pResult->GetErrorMessage());
        }
        sageMgr.Log(strJobDone);

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

    CString strSummary;
    if (pSummary->IsCancelled()) {
        strSummary.Format(_T("[Batch] 취소됨 — 완료 %d건, 실패 %d건"),
                          pSummary->GetSuccess(), pSummary->GetFailed());
    } else {
        strSummary.Format(_T("[Batch] 전체 완료 — 총 %d건, 성공 %d건, 실패 %d건"),
                          pSummary->GetTotal(), pSummary->GetSuccess(), pSummary->GetFailed());
    }
    sageMgr.Log(strSummary);

    // WM_BATCH_COMPLETE: 수신측이 pSummary를 delete해야 한다
    if (::IsWindow(m_hNotify)) {
        ::PostMessage(m_hNotify, WM_BATCH_COMPLETE, 0, (LPARAM)pSummary);
    } else {
        delete pSummary;
    }
}
