
#pragma once

#include <wininet.h>

// ============================================================
// FtpUploadDialog — FTP 업로드 설정 다이얼로그
// ============================================================
class FtpUploadDialog : public CDialogEx
{
public:
    explicit FtpUploadDialog(CWnd* pParent = nullptr);

    CString        m_strHost;
    INTERNET_PORT  m_nPort;
    CString        m_strUser;
    CString        m_strPass;
    CString        m_strRemoteDir;
    CString        m_strFilename;

protected:
    virtual void DoDataExchange(CDataExchange* pDX) override;
    virtual BOOL OnInitDialog() override;
    virtual void OnOK() override;

    DECLARE_MESSAGE_MAP()
};
