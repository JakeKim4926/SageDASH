
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#include "resource.h"

class CSAGEDashApp : public CWinAppEx
{
public:
	CSAGEDashApp() noexcept;

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual void PreLoadState();

	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	DECLARE_MESSAGE_MAP()
};

extern CSAGEDashApp theApp;
