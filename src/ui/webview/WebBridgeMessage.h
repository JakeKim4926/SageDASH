#pragma once

// Forward-declared types used in builder signatures
class ValidationResult;
class DataSheet;

// ---------------------------------------------------------------
// WebBridgeMessage
//   WebView2 양방향 브릿지 JSON 메시지 빌더 / 파서.
//
//   모든 메시지 포맷:
//   { "type": "<타입>", "version": "1.0", "payload": { ... } }
//
//   메시지 타입 상수 (C++ → Web):
//   - BRIDGE_TYPE_NATIVE_READY        WebView2 초기화 완료 알림
//   - BRIDGE_TYPE_DATA_SUMMARY        열린 파일 요약
//   - BRIDGE_TYPE_VALIDATION_REPORT   검증 결과 리포트
//   - BRIDGE_TYPE_COLUMN_ANALYSIS     컬럼 분석 결과
//
//   메시지 타입 상수 (Web → C++):
//   - BRIDGE_TYPE_WEB_REQUEST_SUMMARY    웹 UI가 데이터 요약 요청
//   - BRIDGE_TYPE_WEB_REQUEST_REPORT     웹 UI가 검증 리포트 요청
//   - BRIDGE_TYPE_WEB_REQUEST_ANALYSIS   웹 UI가 컬럼 분석 요청
//   - BRIDGE_TYPE_WEB_ACTION             웹 UI 버튼 액션 트리거
// ---------------------------------------------------------------

// C++ → Web
constexpr LPCTSTR BRIDGE_TYPE_NATIVE_READY          = _T("native:ready");
constexpr LPCTSTR BRIDGE_TYPE_DATA_SUMMARY          = _T("data:summary");
constexpr LPCTSTR BRIDGE_TYPE_VALIDATION_REPORT     = _T("data:validation-report");
constexpr LPCTSTR BRIDGE_TYPE_COLUMN_ANALYSIS       = _T("data:column-analysis");

// Web → C++
constexpr LPCTSTR BRIDGE_TYPE_WEB_REQUEST_SUMMARY   = _T("web:request-summary");
constexpr LPCTSTR BRIDGE_TYPE_WEB_REQUEST_REPORT    = _T("web:request-report");
constexpr LPCTSTR BRIDGE_TYPE_WEB_REQUEST_ANALYSIS  = _T("web:request-analysis");
constexpr LPCTSTR BRIDGE_TYPE_WEB_ACTION            = _T("web:action");

class WebBridgeMessage
{
public:
    // C++ → Web 빌더
    static CString BuildNativeReady();
    static CString BuildDataSummary(int nRowCount, int nColCount, const CString& strFileName);
    static CString BuildValidationReport(const ValidationResult& result);
    static CString BuildColumnAnalysis(const DataSheet& sheet);

    // Web → C++ 파서
    // JSON 문자열에서 "type" 값만 추출. 파싱 실패 시 빈 문자열 반환.
    static CString ParseType(const CString& strJson);

private:
    static CString Build(const CString& strType, const CString& strPayloadJson);
    static CString EscapeString(const CString& strValue);
};
