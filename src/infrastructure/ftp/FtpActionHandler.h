
#pragma once

#include "IActionHandler.h"
#include <wininet.h>

// ============================================================
// FtpActionHandler — WinInet FTP 기반 파일 업로드 액션
// 현재 데이터를 CSV로 변환 후 지정 FTP 서버에 업로드
// ============================================================
class FtpActionHandler : public IActionHandler
{
public:
    FtpActionHandler(const CString& strHost,
                     INTERNET_PORT  nPort,
                     const CString& strUser,
                     const CString& strPass,
                     const CString& strRemoteDir,
                     const CString& strFilename);

    CString GetName() const override;
    void Execute(const TabularData& data) override;

private:
    CString       m_strHost;
    INTERNET_PORT m_nPort;
    CString       m_strUser;
    CString       m_strPass;
    CString       m_strRemoteDir;
    CString       m_strFilename;

    static CString WriteTempCsv(const TabularData& data,
                                 const CString& strFilename);
};
