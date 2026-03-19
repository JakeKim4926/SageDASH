
#include "pch.h"
#include "framework.h"
#include "EmailActionHandler.h"
#include "CsvOutputWriter.h"
#include "FileOutputTarget.h"
#include "SageException.h"
#include <mapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ============================================================
// Simple MAPI를 동적 로드하여 기본 메일 클라이언트 작성창 호출
// 외부 SMTP 설정 불필요, Outlook/Thunderbird 등 모든 클라이언트 지원
// ============================================================

CString EmailActionHandler::GetName() const
{
    return _T("이메일 발송");
}

// ── 임시 CSV 파일 생성 ────────────────────────────────────
CString EmailActionHandler::WriteTempCsv(const TabularData& data)
{
    TCHAR szTempDir[MAX_PATH] = {};
    GetTempPath(MAX_PATH, szTempDir);

    TCHAR szTempFile[MAX_PATH] = {};
    GetTempFileName(szTempDir, _T("SGD"), 0, szTempFile);

    // GetTempFileName이 빈 파일을 만들지만 확장자가 없으므로 .csv 경로 생성
    CString strCsvPath = szTempFile;
    strCsvPath += _T(".csv");

    // 원본 임시 파일 삭제 후 CSV 경로 사용
    DeleteFile(szTempFile);

    FileOutputTarget target(strCsvPath);
    CsvOutputWriter writer;
    writer.Write(data, target);

    return strCsvPath;
}

// ── Simple MAPI 메일 작성창 열기 ──────────────────────────
void EmailActionHandler::LaunchMailCompose(const CString& strAttachPath,
                                            const CString& strSubject)
{
    HMODULE hMapi = LoadLibrary(_T("MAPI32.DLL"));
    if (hMapi == nullptr)
        throw SageException(_T("MAPI32.DLL을 로드할 수 없습니다.\n메일 클라이언트가 설치되어 있는지 확인하세요."));

    typedef ULONG (PASCAL *LPMAPISENDMAILW)(LHANDLE, ULONG_PTR, lpMapiMessageW, FLAGS, ULONG);
    typedef ULONG (PASCAL *LPMAPISENDMAIL) (LHANDLE, ULONG_PTR, lpMapiMessage,  FLAGS, ULONG);

    // MAPISendMailW (Unicode) 우선, 없으면 MAPISendMail (ANSI) 사용
    LPMAPISENDMAILW pfnSendMailW =
        (LPMAPISENDMAILW)GetProcAddress(hMapi, "MAPISendMailW");
    LPMAPISENDMAIL  pfnSendMail  =
        (LPMAPISENDMAIL)GetProcAddress(hMapi, "MAPISendMail");

    ULONG nRet = MAPI_E_FAILURE;

    if (pfnSendMailW != nullptr) {
        // ── Unicode 경로 (MAPISendMailW) ─────────────────────
        MapiFileDescW fileDesc = {};
        fileDesc.nPosition     = (ULONG)-1;
        fileDesc.lpszPathName  = (PWSTR)(LPCWSTR)strAttachPath;

        CString strFileName = strAttachPath;
        int nSlash = strFileName.ReverseFind(_T('\\'));
        if (nSlash >= 0) strFileName = strFileName.Mid(nSlash + 1);
        fileDesc.lpszFileName = (PWSTR)(LPCWSTR)strFileName;

        MapiMessageW msg = {};
        msg.lpszSubject   = (PWSTR)(LPCWSTR)strSubject;
        msg.nFileCount    = 1;
        msg.lpFiles       = &fileDesc;

        nRet = pfnSendMailW(0, 0, &msg, MAPI_DIALOG | MAPI_LOGON_UI, 0);
    } else if (pfnSendMail != nullptr) {
        // ── ANSI fallback (MAPISendMail) ─────────────────────
        // 경로를 CP_ACP로 변환
        int nPathLen = WideCharToMultiByte(CP_ACP, 0,
            strAttachPath, -1, nullptr, 0, nullptr, nullptr);
        std::vector<char> szPath(nPathLen);
        WideCharToMultiByte(CP_ACP, 0,
            strAttachPath, -1, szPath.data(), nPathLen, nullptr, nullptr);

        int nSubjLen = WideCharToMultiByte(CP_ACP, 0,
            strSubject, -1, nullptr, 0, nullptr, nullptr);
        std::vector<char> szSubj(nSubjLen);
        WideCharToMultiByte(CP_ACP, 0,
            strSubject, -1, szSubj.data(), nSubjLen, nullptr, nullptr);

        CString strFileName = strAttachPath;
        int nSlash = strFileName.ReverseFind(_T('\\'));
        if (nSlash >= 0) strFileName = strFileName.Mid(nSlash + 1);
        int nNameLen = WideCharToMultiByte(CP_ACP, 0,
            strFileName, -1, nullptr, 0, nullptr, nullptr);
        std::vector<char> szName(nNameLen);
        WideCharToMultiByte(CP_ACP, 0,
            strFileName, -1, szName.data(), nNameLen, nullptr, nullptr);

        MapiFileDesc fileDesc = {};
        fileDesc.nPosition    = (ULONG)-1;
        fileDesc.lpszPathName = szPath.data();
        fileDesc.lpszFileName = szName.data();

        MapiMessage msg = {};
        msg.lpszSubject  = szSubj.data();
        msg.nFileCount   = 1;
        msg.lpFiles      = &fileDesc;

        nRet = pfnSendMail(0, 0, &msg, MAPI_DIALOG | MAPI_LOGON_UI, 0);
    }

    FreeLibrary(hMapi);

    if (nRet != SUCCESS_SUCCESS && nRet != MAPI_E_USER_ABORT) {
        CString strErr;
        strErr.Format(_T("메일 발송 창 열기 실패 (MAPI 오류 코드: %lu)"), nRet);
        throw SageException(strErr);
    }
}

// ── Execute ───────────────────────────────────────────────
void EmailActionHandler::Execute(const TabularData& data)
{
    // 임시 CSV 파일 생성
    CString strTempCsv = WriteTempCsv(data);

    // 제목 기본값: 파일명
    CString strSubject = _T("SAGEDash 데이터");
    if (!data.m_strFilePath.IsEmpty()) {
        int nSlash = data.m_strFilePath.ReverseFind(_T('\\'));
        int nDot   = data.m_strFilePath.ReverseFind(_T('.'));
        if (nSlash >= 0 && nDot > nSlash)
            strSubject = data.m_strFilePath.Mid(nSlash + 1, nDot - nSlash - 1);
        else if (nSlash >= 0)
            strSubject = data.m_strFilePath.Mid(nSlash + 1);
    }

    // 메일 작성창 열기
    LaunchMailCompose(strTempCsv, strSubject);

    // 임시 파일 정리
    DeleteFile(strTempCsv);
}
