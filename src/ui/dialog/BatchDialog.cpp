
#include "pch.h"
#include "framework.h"
#include "BatchDialog.h"
#include "Resource.h"
#include "Define.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(BatchDialog, CDialogEx)
    ON_BN_CLICKED(IDC_BTN_ADD_FILES,    &BatchDialog::OnBnClickedAddFiles)
    ON_BN_CLICKED(IDC_BTN_REMOVE_FILE,  &BatchDialog::OnBnClickedRemoveFile)
    ON_BN_CLICKED(IDC_BTN_CLEAR_FILES,  &BatchDialog::OnBnClickedClearFiles)
    ON_BN_CLICKED(IDC_BTN_START_BATCH,  &BatchDialog::OnBnClickedStart)
    ON_MESSAGE(WM_BATCH_PROGRESS, &BatchDialog::OnBatchProgress)
    ON_MESSAGE(WM_BATCH_JOB_DONE, &BatchDialog::OnBatchJobDone)
    ON_MESSAGE(WM_BATCH_COMPLETE, &BatchDialog::OnBatchComplete)
END_MESSAGE_MAP()

BatchDialog::BatchDialog(const AutomationProject& project, CWnd* pParent)
    : CDialogEx(IDD_BATCH, pParent)
    , m_project(project)
{
}

BOOL BatchDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    CProgressCtrl* pProgress = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS_BATCH);
    if (pProgress)
        pProgress->SetRange(0, 100);

    SetRunningState(FALSE);
    return TRUE;
}

void BatchDialog::OnCancel()
{
    if (m_runner.IsRunning()) {
        m_runner.RequestCancel();
        return;
    }
    CDialogEx::OnCancel();
}

void BatchDialog::OnBnClickedAddFiles()
{
    CString strFilter;
    strFilter.LoadString(IDS_BATCH_FILE_FILTER);

    CFileDialog dlg(TRUE, nullptr, nullptr,
                    OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
                    strFilter, this);

    // 다중 파일 선택을 위한 버퍼 확장
    const int BUF_SIZE = 32768;
    CString strBuf;
    dlg.GetOFN().lpstrFile = strBuf.GetBuffer(BUF_SIZE);
    dlg.GetOFN().nMaxFile  = BUF_SIZE;

    if (dlg.DoModal() == IDOK) {
        CListBox* pList = (CListBox*)GetDlgItem(IDC_LIST_BATCH_FILES);
        POSITION pos = dlg.GetStartPosition();
        while (pos) {
            CString strPath = dlg.GetNextPathName(pos);
            // 중복 제거
            if (pList->FindStringExact(-1, strPath) == LB_ERR)
                pList->AddString(strPath);
        }
    }

    strBuf.ReleaseBuffer();
}

void BatchDialog::OnBnClickedRemoveFile()
{
    CListBox* pList = (CListBox*)GetDlgItem(IDC_LIST_BATCH_FILES);
    int nSel = pList->GetCurSel();
    if (nSel != LB_ERR)
        pList->DeleteString(nSel);
}

void BatchDialog::OnBnClickedClearFiles()
{
    CListBox* pList = (CListBox*)GetDlgItem(IDC_LIST_BATCH_FILES);
    pList->ResetContent();
}

void BatchDialog::OnBnClickedStart()
{
    CListBox* pList = (CListBox*)GetDlgItem(IDC_LIST_BATCH_FILES);
    int nCount = pList->GetCount();
    if (nCount <= 0) {
        AfxMessageBox(_T("실행할 파일을 추가하세요."), MB_OK | MB_ICONWARNING);
        return;
    }

    std::vector<BatchJob> arrJobs;
    for (int i = 0; i < nCount; i++) {
        CString strPath;
        pList->GetText(i, strPath);
        arrJobs.push_back(BatchJob(strPath, m_project));
    }

    CListBox* pLog = (CListBox*)GetDlgItem(IDC_LIST_BATCH_LOG);
    pLog->ResetContent();
    SetDlgItemText(IDC_STATIC_BATCH_SUMMARY, _T(""));

    CProgressCtrl* pProgress = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS_BATCH);
    if (pProgress) {
        pProgress->SetRange(0, nCount);
        pProgress->SetPos(0);
    }

    CString strStart;
    strStart.LoadString(IDS_LOG_BATCH_START);
    AppendLog(strStart);

    SetRunningState(TRUE);
    m_runner.Start(arrJobs, m_hWnd);
}

LRESULT BatchDialog::OnBatchProgress(WPARAM wParam, LPARAM lParam)
{
    int nDone  = (int)wParam;
    int nTotal = (int)lParam;

    CProgressCtrl* pProgress = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS_BATCH);
    if (pProgress)
        pProgress->SetPos(nDone);

    return 0;
}

LRESULT BatchDialog::OnBatchJobDone(WPARAM wParam, LPARAM lParam)
{
    BatchResult* pResult = reinterpret_cast<BatchResult*>(lParam);
    if (!pResult)
        return 0;

    CString strMsg;
    if (pResult->IsSuccess()) {
        CString strFmt;
        strFmt.LoadString(IDS_LOG_BATCH_JOB_OK);
        strMsg.Format(strFmt, (LPCTSTR)pResult->GetInputPath());
    } else {
        CString strFmt;
        strFmt.LoadString(IDS_LOG_BATCH_JOB_FAIL);
        strMsg.Format(strFmt,
                      (LPCTSTR)pResult->GetInputPath(),
                      (LPCTSTR)pResult->GetErrorMessage());
    }
    AppendLog(strMsg);

    delete pResult;
    return 0;
}

LRESULT BatchDialog::OnBatchComplete(WPARAM wParam, LPARAM lParam)
{
    BatchSummary* pSummary = reinterpret_cast<BatchSummary*>(lParam);
    if (!pSummary)
        return 0;

    SetRunningState(FALSE);

    if (pSummary->IsCancelled()) {
        CString strCancelled;
        strCancelled.LoadString(IDS_LOG_BATCH_CANCELLED);
        AppendLog(strCancelled);
    } else {
        CString strFmt;
        strFmt.LoadString(IDS_LOG_BATCH_DONE);
        CString strDone;
        strDone.Format(strFmt,
                       pSummary->GetTotal(),
                       pSummary->GetSuccess(),
                       pSummary->GetFailed());
        AppendLog(strDone);
    }

    CString strSummaryFmt;
    strSummaryFmt.LoadString(IDS_BATCH_SUMMARY_FMT);
    CString strSummary;
    strSummary.Format(strSummaryFmt,
                      pSummary->GetTotal(),
                      pSummary->GetSuccess(),
                      pSummary->GetFailed());
    if (pSummary->IsCancelled()) {
        CString strCancelLabel;
        strCancelLabel.LoadString(IDS_BATCH_SUMMARY_CANCELLED);
        strSummary += _T("  [") + strCancelLabel + _T("]");
    }
    SetDlgItemText(IDC_STATIC_BATCH_SUMMARY, strSummary);

    delete pSummary;
    return 0;
}

void BatchDialog::SetRunningState(BOOL isRunning)
{
    GetDlgItem(IDC_BTN_ADD_FILES)->EnableWindow(!isRunning);
    GetDlgItem(IDC_BTN_REMOVE_FILE)->EnableWindow(!isRunning);
    GetDlgItem(IDC_BTN_CLEAR_FILES)->EnableWindow(!isRunning);
    GetDlgItem(IDC_BTN_START_BATCH)->EnableWindow(!isRunning);

    CWnd* pClose = GetDlgItem(IDCANCEL);
    if (pClose)
        pClose->SetWindowText(isRunning ? _T("Cancel") : _T("Close"));
}

void BatchDialog::AppendLog(const CString& strMessage)
{
    CListBox* pLog = (CListBox*)GetDlgItem(IDC_LIST_BATCH_LOG);
    if (!pLog)
        return;
    int nIdx = pLog->AddString(strMessage);
    pLog->SetTopIndex(nIdx);
}
