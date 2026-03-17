#pragma once

// ---------------------------------------------------------------
// WebBridgeMessage
//   WebView2 양방향 브릿지 JSON 메시지 빌더 / 파서.
//
//   모든 메시지 포맷:
//   { "type": "<타입>", "version": "1.0", "payload": { ... } }
//
//   C++ → Web 메시지 타입:
//   - "native:ready"   WebView2 초기화 완료 알림
//   - "data:summary"   열린 파일 요약 (Phase 4-7 실제 연동)
//
//   Web → C++ 메시지 타입:
//   - "web:request-summary"  웹 UI가 데이터 요약 요청
//   - "web:action"           웹 UI 버튼 액션 트리거 (Phase 4-7)
// ---------------------------------------------------------------
class WebBridgeMessage
{
public:
    // C++ → Web 빌더
    static CString BuildNativeReady();
    static CString BuildDataSummary(int nRowCount, int nColCount, const CString& strFileName);

    // Web → C++ 파서
    // JSON 문자열에서 "type" 값만 추출. 파싱 실패 시 빈 문자열 반환.
    static CString ParseType(const CString& strJson);

private:
    static CString Build(const CString& strType, const CString& strPayloadJson);
    static CString EscapeString(const CString& strValue);
};
