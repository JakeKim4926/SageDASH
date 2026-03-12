
#pragma once

// SageLogger: 날짜별 파일 로그 기록
// 로그 경로: {exe경로}\logs\yyyy\mm\yyyy-mm-dd.log
// 파일이 없으면 생성, 있으면 이어서 작성
class SageLogger
{
public:
    SageLogger() {}
    ~SageLogger() {}

    BOOL Write(const CString& strMessage);

private:
    CString BuildLogFilePath(const SYSTEMTIME& st) const;
    CString GetExeDirectory() const;
    void    EnsureDirectoryExists(const CString& strDirPath) const;
};
