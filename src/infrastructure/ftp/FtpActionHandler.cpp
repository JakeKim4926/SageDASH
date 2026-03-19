
#include "pch.h"
#include "framework.h"
#include "FtpActionHandler.h"
#include "CsvOutputWriter.h"
#include "FileOutputTarget.h"
#include "SageException.h"
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ============================================================
// FtpActionHandler — WinInet FTP 업로드
// ============================================================

FtpActionHandler::FtpActionHandler(const CString& strHost,
                                    INTERNET_PORT  nPort,
                                    const CString& strUser,
                                    const CString& strPass,
                                    const CString& strRemoteDir,
                                    const CString& strFilename)
    : m_strHost(strHost)
    , m_nPort(nPort)
    , m_strUser(strUser)
    , m_strPass(strPass)
    , m_strRemoteDir(strRemoteDir)
    , m_strFilename(strFilename)
{
}

CString FtpActionHandler::GetName() const
{
    return _T("FTP 업로드");
}

// ── 임시 CSV 파일 생성 ────────────────────────────────────
CString FtpActionHandler::WriteTempCsv(const TabularData& data,
                                        const CString& strFilename)
{
    TCHAR szTempDir[MAX_PATH] = {};
    GetTempPath(MAX_PATH, szTempDir);

    CString strLocalPath = szTempDir;
    strLocalPath += strFilename;

    FileOutputTarget target(strLocalPath);
    CsvOutputWriter writer;
    writer.Write(data, target);

    return strLocalPath;
}

// ── Execute ───────────────────────────────────────────────
void FtpActionHandler::Execute(const TabularData& data)
{
    // 업로드할 파일명 결정
    CString strFilename = m_strFilename;
    if (strFilename.IsEmpty()) {
        if (!data.m_strFilePath.IsEmpty()) {
            int nSlash = data.m_strFilePath.ReverseFind(_T('\\'));
            int nDot   = data.m_strFilePath.ReverseFind(_T('.'));
            if (nSlash >= 0 && nDot > nSlash)
                strFilename = data.m_strFilePath.Mid(nSlash + 1,
                                                     nDot - nSlash - 1);
            else
                strFilename = _T("export");
        } else {
            strFilename = _T("export");
        }
        strFilename += _T(".csv");
    }

    // 임시 CSV 생성
    CString strLocalPath = WriteTempCsv(data, strFilename);

    // WinInet FTP 연결
    HINTERNET hInternet = InternetOpen(
        _T("SAGEDash/1.0"),
        INTERNET_OPEN_TYPE_PRECONFIG,
        nullptr, nullptr, 0);
    if (hInternet == nullptr) {
        DeleteFile(strLocalPath);
        throw SageException(_T("인터넷 연결 핸들 생성에 실패했습니다."));
    }

    HINTERNET hFtp = InternetConnect(
        hInternet,
        m_strHost,
        m_nPort,
        m_strUser.IsEmpty() ? _T("anonymous") : (LPCTSTR)m_strUser,
        m_strPass.IsEmpty() ? _T("guest@")    : (LPCTSTR)m_strPass,
        INTERNET_SERVICE_FTP,
        INTERNET_FLAG_PASSIVE,
        0);

    if (hFtp == nullptr) {
        DWORD dwErr = GetLastError();
        InternetCloseHandle(hInternet);
        DeleteFile(strLocalPath);
        CString strMsg;
        strMsg.Format(_T("FTP 서버 연결에 실패했습니다. (오류: %lu)\n호스트: %s"),
                      dwErr, (LPCTSTR)m_strHost);
        throw SageException(strMsg);
    }

    // 원격 디렉터리 이동
    if (!m_strRemoteDir.IsEmpty()) {
        if (!FtpSetCurrentDirectory(hFtp, m_strRemoteDir)) {
            DWORD dwErr = GetLastError();
            InternetCloseHandle(hFtp);
            InternetCloseHandle(hInternet);
            DeleteFile(strLocalPath);
            CString strMsg;
            strMsg.Format(_T("FTP 디렉터리 이동에 실패했습니다. (오류: %lu)\n경로: %s"),
                          dwErr, (LPCTSTR)m_strRemoteDir);
            throw SageException(strMsg);
        }
    }

    // 파일 업로드
    BOOL bOk = FtpPutFile(
        hFtp,
        strLocalPath,                           // 로컬 경로
        strFilename,                            // 원격 파일명
        FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD,
        0);

    DWORD dwErr = bOk ? 0 : GetLastError();

    InternetCloseHandle(hFtp);
    InternetCloseHandle(hInternet);
    DeleteFile(strLocalPath);

    if (!bOk) {
        CString strMsg;
        strMsg.Format(_T("FTP 파일 업로드에 실패했습니다. (오류: %lu)\n파일: %s"),
                      dwErr, (LPCTSTR)strFilename);
        throw SageException(strMsg);
    }
}
