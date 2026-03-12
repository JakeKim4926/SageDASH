
#include "pch.h"
#include "framework.h"
#include "SageLogger.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString SageLogger::GetExeDirectory() const
{
    TCHAR szPath[MAX_PATH];
    GetModuleFileName(NULL, szPath, MAX_PATH);

    CString strPath(szPath);
    int nSlash = strPath.ReverseFind(_T('\\'));
    if (nSlash >= 0)
        strPath = strPath.Left(nSlash);

    return strPath;
}

CString SageLogger::BuildLogFilePath(const SYSTEMTIME& st) const
{
    CString strDir;
    strDir.Format(_T("%s\\logs\\%04d\\%02d"),
        (LPCTSTR)GetExeDirectory(),
        st.wYear, st.wMonth);

    EnsureDirectoryExists(strDir);

    CString strFile;
    strFile.Format(_T("%s\\%04d-%02d-%02d.log"),
        (LPCTSTR)strDir,
        st.wYear, st.wMonth, st.wDay);

    return strFile;
}

void SageLogger::EnsureDirectoryExists(const CString& strDirPath) const
{
    // 경로의 각 구성 요소를 순서대로 생성 (이미 존재하면 무시)
    for (int i = 0; i < strDirPath.GetLength(); i++) {
        if (strDirPath[i] == _T('\\')) {
            CString strPartial = strDirPath.Left(i);
            if (!strPartial.IsEmpty())
                CreateDirectory(strPartial, NULL);
        }
    }
    CreateDirectory(strDirPath, NULL);
}

BOOL SageLogger::Write(const CString& strMessage)
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    CString strFilePath = BuildLogFilePath(st);

    CStdioFile file;
    if (!file.Open(strFilePath,
        CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeText | CFile::shareDenyNone))
        return FALSE;

    file.SeekToEnd();

    CString strEntry;
    strEntry.Format(_T("[%04d-%02d-%02d %02d:%02d:%02d.%03d] %s\n"),
        st.wYear, st.wMonth, st.wDay,
        st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
        (LPCTSTR)strMessage);

    file.WriteString(strEntry);
    file.Close();
    return TRUE;
}
