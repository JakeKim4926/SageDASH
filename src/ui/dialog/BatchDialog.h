
#pragma once

#include "BatchRunner.h"
#include "AutomationProject.h"
#include "BatchSummary.h"

// ============================================================
// BatchDialog — 배치 실행 다이얼로그 (Phase 6-A)
//
// 사용 방법:
//   BatchDialog dlg(project, pParent);
//   dlg.DoModal();
//
// 동작:
//   1. 입력 파일 목록을 추가 (다중 선택 파일 열기)
//   2. Start 클릭 → BatchRunner 워커 스레드 시작
//   3. WM_BATCH_PROGRESS / WM_BATCH_JOB_DONE / WM_BATCH_COMPLETE 수신
//   4. 실행 완료 또는 취소 후 Close로 닫기
// ============================================================
class BatchDialog : public CDialogEx
{
public:
    explicit BatchDialog(const AutomationProject& project,
                         CWnd* pParent = nullptr);

protected:
    virtual BOOL OnInitDialog() override;
    virtual void OnCancel() override;

    afx_msg void OnBnClickedAddFiles();
    afx_msg void OnBnClickedRemoveFile();
    afx_msg void OnBnClickedClearFiles();
    afx_msg void OnBnClickedStart();
    afx_msg LRESULT OnBatchProgress(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnBatchJobDone(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnBatchComplete(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()

private:
    void SetRunningState(BOOL isRunning);
    void AppendLog(const CString& strMessage);

    AutomationProject m_project;
    BatchRunner       m_runner;
    BOOL              m_isCancelRequested; // 취소 요청 중 여부 (중복 요청 방지)
};
