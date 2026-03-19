
#include "pch.h"
#include "framework.h"
#include <afxrich.h>
#include "OutputWnd.h"
#include "Define.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

constexpr int MAX_LOG_LINES  = 5000;
constexpr int TRIM_TO_LINES  = 4000;
constexpr int LOG_FONT_SIZE  = -11;   // 11pt, negative = pixel height

OutputWnd::OutputWnd() noexcept
{
}

OutputWnd::~OutputWnd()
{
}

BEGIN_MESSAGE_MAP(OutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int OutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	AfxInitRichEdit2();

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndRich.Create(
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
			rectDummy, this, 1))
	{
		TRACE0("로그 창을 만들지 못했습니다.\n");
		return -1;
	}

	m_wndRich.SetBackgroundColor(FALSE, COLOR_WHITE);
	m_wndRich.SetEventMask(ENM_NONE);

	UpdateFonts();
	return 0;
}

void OutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if (m_wndRich.GetSafeHwnd() != nullptr)
		m_wndRich.SetWindowPos(nullptr, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
}

void OutputWnd::UpdateFonts()
{
	if (m_fontLog.GetSafeHandle() != nullptr)
		m_fontLog.DeleteObject();

	// Consolas — Windows 기본 모노스페이스, Cascadia Code 없는 환경에서도 안전
	m_fontLog.CreateFont(
		LOG_FONT_SIZE, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY, FIXED_PITCH | FF_MODERN,
		_T("Consolas"));

	if (m_wndRich.GetSafeHwnd() != nullptr) {
		m_wndRich.SetFont(&m_fontLog);

		// 기본 char format도 갱신
		CHARFORMAT2 cf  = {};
		cf.cbSize       = sizeof(cf);
		cf.dwMask       = CFM_FACE | CFM_SIZE | CFM_COLOR | CFM_EFFECTS | CFM_CHARSET;
		cf.dwEffects    = 0;
		cf.yHeight      = 11 * 20;  // twips (1pt = 20 twips)
		cf.crTextColor  = COLOR_TEXT_MID;
		cf.bCharSet     = DEFAULT_CHARSET;
		_tcscpy_s(cf.szFaceName, LF_FACESIZE, _T("Consolas"));
		m_wndRich.SetDefaultCharFormat(cf);
	}
}

void OutputWnd::AppendLog(const CString& strMessage)
{
	if (m_wndRich.GetSafeHwnd() == nullptr)
		return;

	SYSTEMTIME st;
	GetLocalTime(&st);

	CString strTimestamp;
	strTimestamp.Format(_T("[%04d-%02d-%02d %02d:%02d:%02d.%03d] "),
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

	// 커서를 끝으로 이동
	long nLen = m_wndRich.GetTextLength();
	m_wndRich.SetSel(nLen, nLen);

	// 타임스탬프 — ghost 색상
	CHARFORMAT2 cfTs = {};
	cfTs.cbSize      = sizeof(cfTs);
	cfTs.dwMask      = CFM_COLOR | CFM_EFFECTS | CFM_FACE | CFM_SIZE;
	cfTs.dwEffects   = 0;
	cfTs.yHeight     = 11 * 20;
	cfTs.crTextColor = COLOR_TEXT_GHOST;
	_tcscpy_s(cfTs.szFaceName, LF_FACESIZE, _T("Consolas"));
	m_wndRich.SetSelectionCharFormat(cfTs);
	m_wndRich.ReplaceSel(strTimestamp, FALSE);

	// 메시지 — mid 색상
	nLen = m_wndRich.GetTextLength();
	m_wndRich.SetSel(nLen, nLen);

	CHARFORMAT2 cfMsg = {};
	cfMsg.cbSize      = sizeof(cfMsg);
	cfMsg.dwMask      = CFM_COLOR | CFM_EFFECTS | CFM_FACE | CFM_SIZE;
	cfMsg.dwEffects   = 0;
	cfMsg.yHeight     = 11 * 20;
	cfMsg.crTextColor = COLOR_TEXT_MID;
	_tcscpy_s(cfMsg.szFaceName, LF_FACESIZE, _T("Consolas"));
	m_wndRich.SetSelectionCharFormat(cfMsg);
	m_wndRich.ReplaceSel(strMessage + _T("\r\n"), FALSE);

	// 자동 스크롤
	m_wndRich.SendMessage(WM_VSCROLL, SB_BOTTOM, 0);

	TrimLogIfNeeded();
}

void OutputWnd::TrimLogIfNeeded()
{
	int nLines = m_wndRich.GetLineCount();
	if (nLines <= MAX_LOG_LINES)
		return;

	int nTrimLines = nLines - TRIM_TO_LINES;
	int nCharIdx   = m_wndRich.LineIndex(nTrimLines);
	if (nCharIdx < 0)
		return;

	m_wndRich.SetSel(0, nCharIdx);
	m_wndRich.ReplaceSel(_T(""), FALSE);
}
