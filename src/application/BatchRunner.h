
#pragma once

#include "BatchJob.h"
#include "BatchSummary.h"
#include <vector>

// ============================================================
// BatchRunner — 배치 작업 큐를 워커 스레드에서 순차 실행 (Phase 6-A)
//
// 사용 방법:
//   1. Start(arrJobs, hNotify) 호출 → 워커 스레드 시작
//   2. 워커 스레드가 각 작업을 실행하며 hNotify 윈도우에 메시지 전송:
//      - WM_BATCH_PROGRESS  : wParam=완료건수, lParam=전체건수
//      - WM_BATCH_JOB_DONE  : wParam=인덱스, lParam=new BatchResult*(수신측 delete)
//      - WM_BATCH_COMPLETE  : lParam=new BatchSummary*(수신측 delete)
//   3. 취소 원하면 RequestCancel() 호출
//   4. IsRunning()으로 실행 중 여부 확인 가능
//
// 주의: Start() 호출 전 IsRunning()이 FALSE인지 확인할 것
// ============================================================
class BatchRunner
{
public:
    BatchRunner();

    void Start(const std::vector<BatchJob>& arrJobs, HWND hNotify);
    void RequestCancel();
    BOOL IsRunning() const;

private:
    static UINT __cdecl RunThread(LPVOID pParam);
    void RunInternal();

    std::vector<BatchJob> m_arrJobs;
    HWND                  m_hNotify;
    volatile LONG         m_nCancelFlag;  // 0=실행 중, 1=취소 요청
    volatile LONG         m_nRunningFlag; // 0=미실행, 1=실행 중
};
