# WebView2 브릿지 메시지 규약

**최초 작성**: 2026-03-17 (Phase 4-4)
**최종 확정**: 2026-03-17 (Phase 4-6)
**관련 Phase**: Phase 4-4 / 4-5 / 4-6

---

## 기본 포맷

모든 브릿지 메시지는 아래 JSON 구조를 따른다.

```json
{
  "type": "<메시지 타입>",
  "version": "1.0",
  "payload": { ... }
}
```

| 필드 | 타입 | 설명 |
|------|------|------|
| `type` | string | 메시지 식별자. 방향 접두사 포함 |
| `version` | string | 규약 버전. 현재 `"1.0"` |
| `payload` | object | 타입별 데이터. 없으면 `{}` |

---

## 방향 접두사 규칙

| 접두사 | 방향 | 예시 |
|--------|------|------|
| `native:` | C++ → Web | `native:ready` |
| `data:` | C++ → Web | `data:summary` |
| `web:` | Web → C++ | `web:request-summary` |

---

## C++ → Web 메시지 목록

### `native:ready`
WebView2 초기화 완료 알림. 웹 UI는 이 메시지를 수신한 후 데이터 요청을 시작한다.

```json
{
  "type": "native:ready",
  "version": "1.0",
  "payload": {}
}
```

**발송 시점**: `WebViewHost::OnInitCompleted()` 완료 직후
**C++ 빌더**: `WebBridgeMessage::BuildNativeReady()`

---

### `data:summary`
현재 열린 문서의 데이터 요약. Phase 4-7에서 실제 연동.

```json
{
  "type": "data:summary",
  "version": "1.0",
  "payload": {
    "rowCount": 1500,
    "colCount": 8,
    "fileName": "sales_2026.csv"
  }
}
```

**발송 시점**: 문서 데이터 로드 완료 후 (Phase 4-7 연동 예정)
**C++ 빌더**: `WebBridgeMessage::BuildDataSummary(nRowCount, nColCount, strFileName)`

---

## Web → C++ 메시지 목록

### `web:request-summary`
웹 UI가 `native:ready` 수신 후 데이터 요약을 요청한다.

```json
{
  "type": "web:request-summary",
  "version": "1.0",
  "payload": {}
}
```

**Web 발송 코드**:
```js
window.chrome.webview.postMessage(
  JSON.stringify({ type: 'web:request-summary', version: '1.0', payload: {} })
);
```

**C++ 수신 흐름**: `WebViewHost::OnWebMessageReceived()` → `WM_WEBBRIDGE_MESSAGE` → `SAGEDashView::OnWebBridgeMessage()`
**C++ 처리**: Phase 4-7에서 `BuildDataSummary` 응답 발송 예정

---

### `web:action`
웹 UI 버튼/인터랙션 액션 트리거. Phase 4-7에서 구체화.

```json
{
  "type": "web:action",
  "version": "1.0",
  "payload": {
    "action": "open-file"
  }
}
```

---

## 구현 위치

| 구성 요소 | 파일 | 비고 |
|-----------|------|------|
| JSON 빌더 / 파서 (C++) | `src/ui/webview/WebBridgeMessage.h\|cpp` | `Build*()`, `ParseType()` |
| 메시지 발송 (C++) | `WebViewHost::PostWebMessage()` | `ICoreWebView2::PostWebMessageAsJson` |
| 메시지 수신 (C++) | `WebViewHost::RegisterWebMessageHandler()` | `add_WebMessageReceived` |
| 메시지 디스패치 (C++) | `SAGEDashView::OnWebBridgeMessage()` | `WM_WEBBRIDGE_MESSAGE` 핸들러 |
| 메시지 수신 (Web) | `window.chrome.webview.addEventListener('message', ...)` | `e.data` = JSON 문자열 |
| 메시지 발송 (Web) | `window.chrome.webview.postMessage(JSON.stringify({...}))` | 문자열로 직렬화 필수 |

---

## Web JS 연동 템플릿

```js
// C++ → Web 수신
window.chrome.webview.addEventListener('message', function(e) {
  const msg = JSON.parse(e.data);
  if (msg.type === 'native:ready') {
    // 초기화 완료 — 데이터 요청 시작
    window.chrome.webview.postMessage(
      JSON.stringify({ type: 'web:request-summary', version: '1.0', payload: {} })
    );
  } else if (msg.type === 'data:summary') {
    // KPI 카드 업데이트
    updateDashboard(msg.payload);
  }
});

// Web → C++ 발송 헬퍼
function sendToNative(type, payload) {
  window.chrome.webview.postMessage(
    JSON.stringify({ type: type, version: '1.0', payload: payload || {} })
  );
}
```

---

## C++ dispatch 패턴 (Phase 4-7 사용 예시)

```cpp
LRESULT CSAGEDashView::OnWebBridgeMessage(WPARAM, LPARAM lParam)
{
    CString* pStrJson = reinterpret_cast<CString*>(lParam);
    if (pStrJson != nullptr) {
        CString strType = WebBridgeMessage::ParseType(*pStrJson);
        if (strType == _T("web:request-summary")) {
            // data:summary 응답 발송
        } else if (strType == _T("web:action")) {
            // 액션 처리
        }
        delete pStrJson;
    }
    return 0;
}
```

---

## 확장 원칙

- 신규 C++ → Web 타입: `WebBridgeMessage::Build*()` 메서드 추가
- 신규 Web → C++ 타입: `SAGEDashView::OnWebBridgeMessage()` dispatch 분기 추가
- payload 필드명은 camelCase
- `version` 필드로 규약 변경 시 하위 호환 관리
