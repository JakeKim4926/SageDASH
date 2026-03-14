
#include "pch.h"
#include "framework.h"
#include "OutputWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COutputWnd::COutputWnd() noexcept
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	if (!m_wndLog.Create(
			LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL,
			rectDummy, this, 1))
	{
		TRACE0("로그 창을 만들지 못했습니다.\n");
		return -1;
	}

	UpdateFonts();
	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	if (m_wndLog.GetSafeHwnd() != nullptr)
		m_wndLog.SetWindowPos(nullptr, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
}

void COutputWnd::AppendLog(const CString& strMessage)
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	CString strEntry;
	strEntry.Format(_T("[%04d-%02d-%02d %02d:%02d:%02d.%03d] %s"),
		st.wYear, st.wMonth, st.wDay,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds,
		(LPCTSTR)strMessage);

	m_wndLog.AddString(strEntry);

	int nCount = m_wndLog.GetCount();
	if (nCount > 0)
		m_wndLog.SetTopIndex(nCount - 1);
}

void COutputWnd::UpdateFonts()
{
	if (m_wndLog.GetSafeHwnd() != nullptr)
		m_wndLog.SetFont(&afxGlobalData.fontRegular);
}
