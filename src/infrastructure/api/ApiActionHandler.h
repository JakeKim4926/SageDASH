
#pragma once

#include "IActionHandler.h"
#include <string>

// ============================================================
// ApiActionHandler — WinHTTP 기반 HTTP/HTTPS API 전송 액션
// TabularData를 JSON 배열로 직렬화하여 지정 URL에 POST/PUT
// ============================================================
class ApiActionHandler : public IActionHandler
{
public:
    explicit ApiActionHandler(const CString& strUrl,
                              const CString& strMethod = _T("POST"));

    CString GetName() const override;
    void Execute(const TabularData& data) override;

private:
    CString m_strUrl;
    CString m_strMethod;

    static std::string SerializeJson(const TabularData& data);
    static void        SendRequest(const CString& strUrl,
                                   const CString& strMethod,
                                   const std::string& strBody);
};
