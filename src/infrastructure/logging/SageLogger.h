
#pragma once

// SageLogger: 날짜별 파일 로그 기록
// 로그 경로: {exe경로}\logs\yyyy\mm\yyyy-mm-dd.log
// 파일이 없으면 생성, 있으면 이어서 작성
// - Write 전체를 CCriticalSection으로 보호 (멀티스레드 안전)
// - 연/월이 바뀔 때만 디렉터리 생성 (캐싱)
class SageLogger
{
public:
    SageLogger();
    ~SageLogger() {}

    BOOL Write(const CString& strMessage);

private:
    CString BuildLogFilePath(const SYSTEMTIME& st);
    CString GetExeDirectory() const;
    void    EnsureDirectoryExists(const CString& strDirPath) const;

    mutable CCriticalSection m_cs;
    WORD m_wLastYear;
    WORD m_wLastMonth;
};
