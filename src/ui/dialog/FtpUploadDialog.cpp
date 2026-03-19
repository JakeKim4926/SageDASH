
#include "pch.h"
#include "framework.h"
#include "FtpUploadDialog.h"
#include "Resource.h"
#include <wininet.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(FtpUploadDialog, CDialogEx)
END_MESSAGE_MAP()

FtpUploadDialog::FtpUploadDialog(CWnd* pParent)
    : CDialogEx(IDD_FTP_UPLOAD, pParent)
    , m_nPort(INTERNET_DEFAULT_FTP_PORT)
{
}

void FtpUploadDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_FTP_HOST,       m_strHost);
    DDX_Text(pDX, IDC_EDIT_FTP_USER,       m_strUser);
    DDX_Text(pDX, IDC_EDIT_FTP_PASS,       m_strPass);
    DDX_Text(pDX, IDC_EDIT_FTP_REMOTE_DIR, m_strRemoteDir);
    DDX_Text(pDX, IDC_EDIT_FTP_FILENAME,   m_strFilename);
}

BOOL FtpUploadDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    CString strPort;
    strPort.Format(_T("%d"), (int)m_nPort);
    SetDlgItemText(IDC_EDIT_FTP_PORT, strPort);

    return TRUE;
}

void FtpUploadDialog::OnOK()
{
    UpdateData(TRUE);

    m_strHost.Trim();
    if (m_strHost.IsEmpty()) {
        AfxMessageBox(_T("FTP 호스트를 입력하세요."), MB_OK | MB_ICONWARNING);
        GetDlgItem(IDC_EDIT_FTP_HOST)->SetFocus();
        return;
    }

    // 포트 파싱
    CString strPort;
    GetDlgItemText(IDC_EDIT_FTP_PORT, strPort);
    int nPort = _ttoi(strPort);
    m_nPort = (nPort > 0 && nPort <= 65535)
              ? (INTERNET_PORT)nPort
              : INTERNET_DEFAULT_FTP_PORT;

    CDialogEx::OnOK();
}
