#pragma once

// ---------------------------------------------------------------
// WebBridgeMessage
//   C++ → Web 단방향 브릿지 JSON 메시지 빌더.
//
//   모든 메시지 포맷:
//   { "type": "<타입>", "version": "1.0", "payload": { ... } }
//
//   메시지 타입 목록:
//   - "native:ready"   WebView2 초기화 완료 알림
//   - "data:summary"   열린 파일 요약 (Phase 4-7 실제 연동)
// ---------------------------------------------------------------
class WebBridgeMessage
{
public:
    static CString BuildNativeReady();
    static CString BuildDataSummary(int nRowCount, int nColCount, const CString& strFileName);

private:
    static CString Build(const CString& strType, const CString& strPayloadJson);
    static CString EscapeString(const CString& strValue);
};
