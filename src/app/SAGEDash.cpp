
// SAGEDash.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "SAGEDash.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "SAGEDashDoc.h"
#include "SAGEDashView.h"
#include "SageMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CSAGEDashApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CSAGEDashApp::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CSAGEDashApp::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
END_MESSAGE_MAP()

CSAGEDashApp::CSAGEDashApp() noexcept
{
	m_bHiColorIcons = TRUE;
	m_nAppLook = 0;
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
	SetAppID(_T("SAGEDash.AppID.NoVersion"));
}

CSAGEDashApp theApp;

BOOL CSAGEDashApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();
	sageMgr.Initialize();
	EnableTaskbarInteraction();

	SetRegistryKey(_T("로컬 애플리케이션 마법사에서 생성된 애플리케이션"));
	LoadStdProfileSettings(4);

	InitContextMenuManager();
	InitKeyboardManager();
	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(IDR_SAGEDashTYPE,
		RUNTIME_CLASS(CSAGEDashDoc),
		RUNTIME_CLASS(CChildFrame),
		RUNTIME_CLASS(CSAGEDashView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CSAGEDashApp::ExitInstance()
{
	sageMgr.Shutdown();
	return CWinAppEx::ExitInstance();
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

void CSAGEDashApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CSAGEDashApp::OnFileOpen()
{
	CFileDialog dlg(
		TRUE,
		_T("csv"),
		nullptr,
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST,
		_T("CSV 파일 (*.csv)|*.csv|모든 파일 (*.*)|*.*||"),
		AfxGetMainWnd());

	if (dlg.DoModal() != IDOK)
		return;

	OpenDocumentFile(dlg.GetPathName());
}

void CSAGEDashApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
}
