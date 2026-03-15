
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
// 검색 바
// ============================================================
constexpr int SEARCH_BAR_HEIGHT = 24;
constexpr int SEARCH_BAR_MARGIN = 4;

// ============================================================
// 그리드 미리보기
// ============================================================
constexpr int MAX_PREVIEW_ROWS          = 500;
constexpr int GRID_ROW_NUM_COLUMN_WIDTH = 45;
constexpr int GRID_COLUMN_WIDTH_DEFAULT = 120;
constexpr int GRID_COLUMN_WIDTH_MIN     = 50;
constexpr int GRID_COLUMN_WIDTH_MAX     = 300;
