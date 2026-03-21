
#pragma once

// ============================================================
// 색상 토큰
// ============================================================
#define COLOR_FRAME             RGB(0xE4, 0xE2, 0xDD)
#define COLOR_FRAME_HI          RGB(0xED, 0xEB, 0xE7)
#define COLOR_FRAME_LO          RGB(0xD8, 0xD5, 0xCF)
#define COLOR_FRAME_BORDER      RGB(0xC8, 0xC4, 0xBC)
#define COLOR_SURFACE           RGB(0xF4, 0xF2, 0xEE)
#define COLOR_SURFACE_ALT       RGB(0xE9, 0xE7, 0xE2)
#define COLOR_WHITE             RGB(0xFD, 0xFC, 0xFA)
#define COLOR_CREAM             RGB(0xF9, 0xF8, 0xF5)
#define COLOR_TEXT              RGB(0x2A, 0x29, 0x26)
#define COLOR_TEXT_MID          RGB(0x5C, 0x58, 0x50)
#define COLOR_TEXT_DIM          RGB(0x8A, 0x85, 0x7C)
#define COLOR_TEXT_GHOST        RGB(0xB0, 0xAC, 0xA4)
#define COLOR_TEXT_MUTED        RGB(0xCB, 0xC7, 0xC0)
#define COLOR_ACCENT            RGB(0x4A, 0x6D, 0x8C)
#define COLOR_ACCENT_LIGHT      RGB(0xE3, 0xEB, 0xF2)
#define COLOR_ACCENT_DARK       RGB(0x35, 0x53, 0x6C)
#define COLOR_SELECTION         RGB(0xD4, 0xE3, 0xF0)
#define COLOR_SELECTION_BORDER  RGB(0x9B, 0xBA, 0xD4)
#define COLOR_ERROR             RGB(0xB8, 0x48, 0x40)
#define COLOR_ERROR_BG          RGB(0xFA, 0xEE, 0xEC)
#define COLOR_SUCCESS           RGB(0x5E, 0x85, 0x48)
#define COLOR_SUCCESS_BG        RGB(0xEE, 0xF3, 0xEA)
#define COLOR_WARNING           RGB(0xA0, 0x7E, 0x28)
#define COLOR_WARNING_BG        RGB(0xF7, 0xF2, 0xE4)
#define COLOR_BORDER            RGB(0xD8, 0xD5, 0xCE)
#define COLOR_BORDER_LIGHT      RGB(0xE5, 0xE2, 0xDC)

// ============================================================
// 사용자 정의 메시지
// ============================================================
#define WM_SWITCH_CENTER_VIEW  (WM_USER + 1)  // wParam = CenterViewMode
#define WM_WEBBRIDGE_MESSAGE   (WM_USER + 2)  // lParam = new CString*(JSON), 수신측 delete
#define WM_BATCH_PROGRESS      (WM_USER + 3)  // wParam = 완료 건수, lParam = 전체 건수
#define WM_BATCH_JOB_DONE      (WM_USER + 4)  // wParam = 작업 인덱스, lParam = new BatchResult*(수신측 delete)
#define WM_BATCH_COMPLETE      (WM_USER + 5)  // lParam = new BatchSummary*(수신측 delete)

// ============================================================
// 공통 컨트롤
// ============================================================
constexpr int COMBO_DROPDOWN_H  = 200;  // ComboBox SetWindowPos 시 드롭다운 높이
constexpr int CHECKBOX_HEIGHT   = 16;   // 체크박스 기본 높이

// ============================================================
// 검색 바
// ============================================================
constexpr int SEARCH_BAR_HEIGHT = 24;
constexpr int SEARCH_BAR_MARGIN = 4;

// ============================================================
// CSV 파싱
// ============================================================
constexpr TCHAR CSV_DELIM_COMMA     = _T(',');
constexpr TCHAR CSV_DELIM_TAB       = _T('\t');
constexpr TCHAR CSV_DELIM_SEMICOLON = _T(';');

// ============================================================
// 그리드 미리보기
// ============================================================
constexpr int MAX_PREVIEW_ROWS          = 500;
constexpr int GRID_ROW_NUM_COLUMN_WIDTH = 45;
constexpr int GRID_COLUMN_WIDTH_DEFAULT = 120;
constexpr int GRID_COLUMN_WIDTH_MIN     = 50;
constexpr int GRID_COLUMN_WIDTH_MAX     = 300;

// ============================================================
// SAGEDashView — 컨트롤 ID
// ============================================================
#define IDC_SEARCH_EDIT      100
#define IDC_MAPPING_PANEL    110
#define IDC_VALIDATION_PANEL 111
#define IDC_WEBVIEW_HOST     120

// ============================================================
// MappingPanel — 컨트롤 ID
// ============================================================
#define IDC_MAP_COMBO_SOURCE    200
#define IDC_MAP_EDIT_TARGET     201
#define IDC_MAP_CHK_REQUIRED    202
#define IDC_MAP_BTN_ADD         203
#define IDC_MAP_BTN_DELETE      204
#define IDC_MAP_BTN_AUTOMAP     205
#define IDC_MAP_BTN_CLEAR       206
#define IDC_MAP_LIST_RULES      207

// ============================================================
// MappingPanel — 레이아웃
// ============================================================
constexpr int MAP_HEADER_H      = 28;
constexpr int MAP_CONTROLS_H    = 24;
constexpr int MAP_BUTTONS_H     = 24;
constexpr int MAP_STATUS_H      = 20;
constexpr int MAP_PADDING       = 8;
constexpr int MAP_ROW_GAP       = 4;
constexpr int MAP_COMBO_W       = 180;
constexpr int MAP_EDIT_W        = 180;
constexpr int MAP_CHK_W         = 48;
constexpr int MAP_BTN_ADD_W     = 48;
constexpr int MAP_BTN_DELETE_W  = 52;
constexpr int MAP_BTN_AUTOMAP_W = 80;
constexpr int MAP_BTN_CLEAR_W   = 52;
constexpr int MAP_BTN_GAP       = 4;
constexpr int MAP_COL_NUM_W     = 40;
constexpr int MAP_COL_SRC_W     = 180;
constexpr int MAP_COL_TGT_W     = 180;
constexpr int MAP_COL_REQ_W     = 50;

// ============================================================
// ValidationPanel — 컨트롤 ID
// ============================================================
#define IDC_VAL_COMBO_COLUMN    210
#define IDC_VAL_COMBO_TYPE      211
#define IDC_VAL_COMBO_SEVERITY  212
#define IDC_VAL_EDIT_PATTERN    213
#define IDC_VAL_BTN_ADD         214
#define IDC_VAL_BTN_DELETE      215
#define IDC_VAL_BTN_CLEAR_RULES 216
#define IDC_VAL_LIST_RULES      217
#define IDC_VAL_BTN_RUN         218
#define IDC_VAL_LIST_RESULTS    219

// ============================================================
// ValidationPanel — 레이아웃
// ============================================================
constexpr int VAL_HEADER_H       = 28;
constexpr int VAL_CONTROLS_H     = 24;
constexpr int VAL_BUTTONS_H      = 24;
constexpr int VAL_RULES_LIST_H   = 140;
constexpr int VAL_RUN_H          = 28;
constexpr int VAL_PADDING        = 8;
constexpr int VAL_ROW_GAP        = 4;
constexpr int VAL_COMBO_ITEM_H   = 18;   // ComboBox 항목 높이
constexpr int VAL_MIN_PATTERN_W  = 40;   // 패턴 필드 최소 너비
constexpr int VAL_BTN_ADD_W      = 48;   // 추가 버튼 너비
constexpr int VAL_BTN_DELETE_W   = 52;   // 삭제 버튼 너비
constexpr int VAL_BTN_CLEAR_W    = 70;   // 전체삭제 버튼 너비
constexpr int VAL_BTN_RUN_W      = 80;   // 실행 버튼 너비
constexpr int VAL_COL_NUM_W      = 30;   // 규칙/결과 목록 번호 컬럼 너비
constexpr int VAL_COL_COLUMN_W   = 120;  // 규칙 목록 컬럼명 컬럼 너비
constexpr int VAL_COL_TYPE_W     = 72;   // 규칙 목록 타입 컬럼 너비
constexpr int VAL_COL_SEVERITY_W = 60;   // 심각도 컬럼 너비
constexpr int VAL_COL_PATTERN_W  = 120;  // 패턴 컬럼 너비
constexpr int VAL_RES_COL_ROW_W  = 50;   // 결과 목록 행 번호 컬럼 너비
constexpr int VAL_RES_COL_COL_W  = 110;  // 결과 목록 컬럼명 컬럼 너비
constexpr int VAL_RES_COL_MSG_W  = 200;  // 결과 목록 메시지 컬럼 너비

// ============================================================
// 폴더 스캔
// ============================================================
constexpr int FOLDER_SCAN_MAX_FILES = 10000;   // 스캔 최대 파일 수

// ============================================================
// DB 조회 (.dbq)
// ============================================================
constexpr int DB_QUERY_MAX_ROWS = 10000;        // 최대 조회 행 수

// ============================================================
// 배치 실행
// ============================================================
constexpr DWORD BATCH_SHUTDOWN_TIMEOUT_MS = 5000;  // 앱/문서 종료 시 배치 완료 대기 타임아웃 (ms)

// ============================================================
// PropertiesPane — 레이아웃
// ============================================================
constexpr int PROP_COL_LABEL_W   = 80;  // 라벨 컬럼 초기 너비
constexpr int PROP_COL_VALUE_W   = 200; // 값 컬럼 초기 너비
constexpr int PROP_SCROLLBAR_ADJ = 2;   // 스크롤바 너비 조정값
