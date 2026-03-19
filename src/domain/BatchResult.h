
#pragma once

// ============================================================
// BatchResult — 배치 작업 단건 실행 결과 (Phase 6-A)
// ============================================================
class BatchResult
{
public:
    BatchResult()
        : m_isSuccess(FALSE)
    {}

    BOOL    IsSuccess()      const { return m_isSuccess; }
    CString GetInputPath()   const { return m_strInputPath; }
    CString GetErrorMessage() const { return m_strErrorMessage; }
    CString GetLog()         const { return m_strLog; }

    void SetSuccess(BOOL b)               { m_isSuccess = b; }
    void SetInputPath(const CString& v)   { m_strInputPath = v; }
    void SetErrorMessage(const CString& v) { m_strErrorMessage = v; }
    void SetLog(const CString& v)         { m_strLog = v; }
    void AppendLog(const CString& v)      { m_strLog += v; }

private:
    BOOL    m_isSuccess;
    CString m_strInputPath;
    CString m_strErrorMessage;
    CString m_strLog;
};
