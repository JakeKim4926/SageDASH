
#pragma once

#include "BatchResult.h"
#include <vector>

// ============================================================
// BatchSummary — 배치 실행 전체 집계 결과 (Phase 6-A)
// WM_BATCH_COMPLETE 메시지의 lParam으로 전달된다 (수신측 delete).
// ============================================================
class BatchSummary
{
public:
    BatchSummary()
        : m_nTotal(0)
        , m_nSuccess(0)
        , m_nFailed(0)
        , m_isCancelled(FALSE)
    {}

    int  GetTotal()     const { return m_nTotal; }
    int  GetSuccess()   const { return m_nSuccess; }
    int  GetFailed()    const { return m_nFailed; }
    BOOL IsCancelled()  const { return m_isCancelled; }

    const std::vector<BatchResult>& GetResults() const { return m_arrResults; }

    void SetTotal(int n)       { m_nTotal = n; }
    void SetCancelled(BOOL b)  { m_isCancelled = b; }

    void AddResult(const BatchResult& result) {
        m_arrResults.push_back(result);
        m_nTotal++;
        if (result.IsSuccess())
            m_nSuccess++;
        else
            m_nFailed++;
    }

private:
    int  m_nTotal;
    int  m_nSuccess;
    int  m_nFailed;
    BOOL m_isCancelled;
    std::vector<BatchResult> m_arrResults;
};
