# WebView2 브릿지 메시지 규약

**날짜**: 2026-03-17
**관련 Phase**: Phase 4-4 / 4-5
**브랜치**: feature/webview2-bridge-cpp-to-web

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
| `web:` | Web → C++ | `web:action` |

---

## C++ → Web 메시지 목록

### `native:ready`
WebView2 초기화 완료 알림. 웹 UI가 이 메시지를 수신한 후 데이터 요청을 시작한다.

```json
{
  "type": "native:ready",
  "version": "1.0",
  "payload": {}
}
```

**발송 시점**: `WebViewHost::OnInitCompleted()` 완료 직후

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

---

## Web → C++ 메시지 목록 (Phase 4-5 정의 예정)

| 타입 | 설명 |
|------|------|
| `web:request-summary` | 웹 UI가 요약 데이터 요청 |
| `web:action` | 웹 UI 버튼 액션 트리거 |

---

## 구현 위치

| 구성 요소 | 파일 |
|-----------|------|
| JSON 빌더 (C++) | `src/ui/webview/WebBridgeMessage.h|cpp` |
| 메시지 발송 (C++) | `src/ui/webview/WebViewHost::PostWebMessage()` |
| 메시지 수신 (C++) | `src/ui/webview/WebViewHost` — Phase 4-5 추가 예정 |
| 메시지 수신 (Web) | `window.chrome.webview.addEventListener('message', ...)` |
| 메시지 발송 (Web) | `window.chrome.webview.postMessage(...)` |

---

## 확장 원칙

- 신규 타입 추가 시 `WebBridgeMessage`에 `Build*` 메서드 추가
- `version` 필드로 규약 변경 시 하위 호환 관리
- payload 필드명은 camelCase
