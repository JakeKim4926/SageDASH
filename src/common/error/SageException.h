
#pragma once

// SageException: SAGEDash 예외 계층의 기반 클래스
// 에러 메시지와 추적 컨텍스트(파일 경로, 문서 ID 등)를 보관한다.
// 로깅은 catch 지점(계층 경계)에서 담당한다.
class SageException
{
public:
    SageException(const CString& strMessage, const CString& strContext = _T(""))
        : m_strMessage(strMessage), m_strContext(strContext) {}

    const CString& GetMessage() const { return m_strMessage; }
    const CString& GetContext() const { return m_strContext; }

    // 로그/UI 출력용 요약 문자열
    CString Format() const {
        if (m_strContext.IsEmpty())
            return m_strMessage;
        CString strFormatted;
        strFormatted.Format(_T("%s [%s]"), (LPCTSTR)m_strMessage, (LPCTSTR)m_strContext);
        return strFormatted;
    }

private:
    CString m_strMessage;
    CString m_strContext;
};
