
// SAGEDashDoc.cpp: CSAGEDashDoc 클래스의 구현

#include "pch.h"
#include "framework.h"
#include "SAGEDash.h"
#include "SAGEDashDoc.h"
#include "MainFrm.h"
#include "WorkbookService.h"
#include "SageException.h"
#include "SageMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CSAGEDashDoc, CDocument)

BEGIN_MESSAGE_MAP(CSAGEDashDoc, CDocument)
END_MESSAGE_MAP()

CSAGEDashDoc::CSAGEDashDoc() noexcept
	: m_isDataLoaded(FALSE)
{
}

CSAGEDashDoc::~CSAGEDashDoc()
{
}

BOOL CSAGEDashDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	DeleteContents();

	CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	WorkbookService service;

	try {
		service.LoadFromFile(lpszPathName, m_data);
		m_isDataLoaded = TRUE;
	} catch (const SageException& e) {
		CString strLog;
		strLog.Format(_T("[실패] %s"), (LPCTSTR)e.Format());
		sageMgr.Log(strLog);
		if (pFrame != nullptr)
			pFrame->LogMessage(strLog);
		AfxMessageBox(e.GetMessage(), MB_OK | MB_ICONWARNING);
		return FALSE;
	}

	const DataSheet& sheet = m_data.GetSheet(0);

	if (sheet.GetRowCount() <= 1) {
		CString strWarn;
		strWarn.Format(_T("[경고] %s — 헤더만 있고 데이터가 없습니다."), lpszPathName);
		sageMgr.Log(strWarn);
		if (pFrame != nullptr)
			pFrame->LogMessage(strWarn);
	}

	CString strLog;
	strLog.Format(_T("[성공] %s 로드 완료 (%d행 × %d열)"),
		lpszPathName,
		sheet.GetRowCount(),
		sheet.GetColumnCount());
	sageMgr.Log(strLog);
	if (pFrame != nullptr) {
		pFrame->LogMessage(strLog);
		pFrame->GetPropertiesPane().SetFileInfo(lpszPathName, m_data);
	}

	SetModifiedFlag(FALSE);
	UpdateAllViews(nullptr);
	return TRUE;
}

void CSAGEDashDoc::DeleteContents()
{
	m_data.Clear();
	m_isDataLoaded = FALSE;

	CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	if (pFrame != nullptr)
		pFrame->GetPropertiesPane().ClearInfo();

	CDocument::DeleteContents();
}

void CSAGEDashDoc::Serialize(CArchive& ar)
{
	UNREFERENCED_PARAMETER(ar);
}

void CSAGEDashDoc::ReportSaveLoadException(LPCTSTR /*lpszPathName*/, CException* /*e*/, BOOL /*bSaving*/, UINT /*nIDPDefault*/)
{
}

#ifdef _DEBUG
void CSAGEDashDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSAGEDashDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG
