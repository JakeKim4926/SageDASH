
#include "pch.h"
#include "framework.h"
#include "ApiActionHandler.h"
#include "SageException.h"
#include <winhttp.h>
#include <vector>
#include <string>

#pragma comment(lib, "winhttp.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ============================================================
// ApiActionHandler — WinHTTP 기반 HTTP/HTTPS API 전송
// ============================================================

ApiActionHandler::ApiActionHandler(const CString& strUrl,
                                    const CString& strMethod)
    : m_strUrl(strUrl)
    , m_strMethod(strMethod)
{
}

CString ApiActionHandler::GetName() const
{
    return _T("API 전송");
}

// ── JSON 직렬화 ───────────────────────────────────────────
// 첫 행 = 헤더 키, 이후 행 = 데이터 → JSON array of objects
static std::string JsonEscape(const CString& str)
{
    // CString(UTF-16) → UTF-8
    int nBytes = WideCharToMultiByte(CP_UTF8, 0,
        (LPCWSTR)(LPCTSTR)str, -1, nullptr, 0, nullptr, nullptr);
    std::vector<char> buf(nBytes);
    WideCharToMultiByte(CP_UTF8, 0,
        (LPCWSTR)(LPCTSTR)str, -1, buf.data(), nBytes, nullptr, nullptr);
    std::string utf8(buf.data(), buf.size() > 0 ? buf.size() - 1 : 0);

    std::string r;
    r.reserve(utf8.size() + 4);
    r += '"';
    for (unsigned char c : utf8) {
        if      (c == '"')  r += "\\\"";
        else if (c == '\\') r += "\\\\";
        else if (c == '\n') r += "\\n";
        else if (c == '\r') r += "\\r";
        else if (c == '\t') r += "\\t";
        else                r += (char)c;
    }
    r += '"';
    return r;
}

std::string ApiActionHandler::SerializeJson(const TabularData& data)
{
    if (data.GetSheetCount() == 0)
        return "[]";

    const DataSheet& sh = data.GetSheet(0);
    int nRows = sh.GetRowCount();
    if (nRows == 0)
        return "[]";

    // 헤더 행
    const std::vector<CString>& headers = sh.m_arrRows[0];
    int nCols = (int)headers.size();

    std::string json;
    json.reserve(nRows * nCols * 32);
    json += "[\n";

    for (int r = 1; r < nRows; r++) {
        const std::vector<CString>& row = sh.m_arrRows[r];
        json += "  {";
        for (int c = 0; c < nCols; c++) {
            if (c > 0) json += ",";
            CString key   = c < (int)headers.size() ? headers[c] : CString();
            CString value = c < (int)row.size()    ? row[c]     : CString();
            json += JsonEscape(key) + ":" + JsonEscape(value);
        }
        json += "}";
        if (r < nRows - 1) json += ",";
        json += "\n";
    }
    json += "]";
    return json;
}

// ── WinHTTP 요청 전송 ─────────────────────────────────────
void ApiActionHandler::SendRequest(const CString& strUrl,
                                    const CString& strMethod,
                                    const std::string& strBody)
{
    // URL 파싱
    URL_COMPONENTS uc = {};
    uc.dwStructSize        = sizeof(uc);
    uc.dwSchemeLength      = (DWORD)-1;
    uc.dwHostNameLength    = (DWORD)-1;
    uc.dwUrlPathLength     = (DWORD)-1;
    uc.dwExtraInfoLength   = (DWORD)-1;

    if (!WinHttpCrackUrl((LPCWSTR)(LPCTSTR)strUrl, 0, 0, &uc)) {
        CString msg;
        msg.Format(_T("잘못된 URL 형식입니다: %s"), (LPCTSTR)strUrl);
        throw SageException(msg);
    }

    CString strHost(uc.lpszHostName, uc.dwHostNameLength);
    CString strPath(uc.lpszUrlPath,  uc.dwUrlPathLength);
    if (!uc.dwExtraInfoLength == 0)
        strPath += CString(uc.lpszExtraInfo, uc.dwExtraInfoLength);
    if (strPath.IsEmpty()) strPath = _T("/");

    BOOL   bHttps = (uc.nScheme == INTERNET_SCHEME_HTTPS);
    DWORD  nPort  = (uc.nPort != 0) ? uc.nPort
                                     : (bHttps ? INTERNET_DEFAULT_HTTPS_PORT
                                               : INTERNET_DEFAULT_HTTP_PORT);

    // WinHTTP 세션
    HINTERNET hSession = WinHttpOpen(
        _T("SAGEDash/1.0"),
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (hSession == nullptr)
        throw SageException(_T("WinHTTP 세션 생성에 실패했습니다."));

    HINTERNET hConnect = WinHttpConnect(hSession, strHost, (INTERNET_PORT)nPort, 0);
    if (hConnect == nullptr) {
        WinHttpCloseHandle(hSession);
        throw SageException(_T("서버 연결에 실패했습니다."), strHost);
    }

    DWORD dwFlags = bHttps ? WINHTTP_FLAG_SECURE : 0;
    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect, (LPCWSTR)(LPCTSTR)strMethod,
        strPath, nullptr,
        WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, dwFlags);
    if (hRequest == nullptr) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        throw SageException(_T("HTTP 요청 생성에 실패했습니다."));
    }

    // Content-Type: application/json; charset=utf-8
    LPCWSTR szHeaders = L"Content-Type: application/json; charset=utf-8";
    BOOL bSent = WinHttpSendRequest(
        hRequest, szHeaders, (DWORD)-1,
        (LPVOID)strBody.c_str(), (DWORD)strBody.size(),
        (DWORD)strBody.size(), 0);

    if (bSent)
        bSent = WinHttpReceiveResponse(hRequest, nullptr);

    DWORD dwStatus = 0;
    if (bSent) {
        DWORD dwStatusSize = sizeof(DWORD);
        WinHttpQueryHeaders(hRequest,
            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            WINHTTP_HEADER_NAME_BY_INDEX, &dwStatus, &dwStatusSize, nullptr);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    if (!bSent) {
        CString msg;
        msg.Format(_T("HTTP 요청 전송에 실패했습니다. (WinHTTP 오류: %lu)"),
                   GetLastError());
        throw SageException(msg, strUrl);
    }

    if (dwStatus < 200 || dwStatus >= 300) {
        CString msg;
        msg.Format(_T("서버가 오류를 반환했습니다. (HTTP %lu)"), dwStatus);
        throw SageException(msg, strUrl);
    }
}

// ── Execute ───────────────────────────────────────────────
void ApiActionHandler::Execute(const TabularData& data)
{
    std::string jsonBody = SerializeJson(data);
    SendRequest(m_strUrl, m_strMethod, jsonBody);
}
