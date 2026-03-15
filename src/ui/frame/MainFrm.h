
// MainFrm.h: CMainFrame 클래스의 인터페이스

#pragma once
#include "OutputWnd.h"
#include "NavigatorPane.h"
#include "PropertiesPane.h"
#include "SageMgr.h"
#include "Define.h"

class CMainFrame : public CMDIFrameWndEx
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame() noexcept;
	virtual ~CMainFrame();

public:
	void LogMessage(const CString& strMessage);
	CPropertiesPane& GetPropertiesPane() { return m_wndProperties; }

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CNavigatorPane    m_wndNavigator;
	CPropertiesPane   m_wndProperties;
	COutputWnd        m_wndOutput;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnWindowManager();
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnViewNavigatorWnd();
	afx_msg void OnUpdateViewNavigatorWnd(CCmdUI* pCmdUI);
	afx_msg void OnViewPropertiesWnd();
	afx_msg void OnUpdateViewPropertiesWnd(CCmdUI* pCmdUI);
	afx_msg void OnViewOutputWnd();
	afx_msg void OnUpdateViewOutputWnd(CCmdUI* pCmdUI);
	afx_msg LRESULT OnSwitchCenterView(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
};
