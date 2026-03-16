
// SAGEDashDoc.cpp: CSAGEDashDoc 클래스의 구현

#include "pch.h"
#include "framework.h"
#include "SAGEDash.h"
#include "SAGEDashDoc.h"
#include "MainFrm.h"
#include "WorkbookService.h"
#include "ExportService.h"
#include "SageException.h"
#include "SageMgr.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CSAGEDashDoc, CDocument)

BEGIN_MESSAGE_MAP(CSAGEDashDoc, CDocument)
    ON_COMMAND(ID_FILE_EXPORT,           &CSAGEDashDoc::OnFileExport)
    ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT, &CSAGEDashDoc::OnUpdateFileExport)
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
		pFrame->GetNavigatorPane().ActivatePipelineItems(TRUE);
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
	if (pFrame != nullptr) {
		pFrame->GetNavigatorPane().ActivatePipelineItems(FALSE);
		pFrame->GetPropertiesPane().ClearInfo();
	}

	CDocument::DeleteContents();
}

void CSAGEDashDoc::Serialize(CArchive& ar)
{
	UNREFERENCED_PARAMETER(ar);
}

void CSAGEDashDoc::ReportSaveLoadException(LPCTSTR /*lpszPathName*/, CException* /*e*/, BOOL /*bSaving*/, UINT /*nIDPDefault*/)
{
}

void CSAGEDashDoc::OnFileExport()
{
    if (!m_isDataLoaded)
        return;

    CString strFilter;
    strFilter.LoadString(IDS_EXPORT_FILE_FILTER);

    CFileDialog dlg(FALSE, _T("csv"), nullptr,
        OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
        strFilter, AfxGetMainWnd());

    if (dlg.DoModal() != IDOK)
        return;

    CString strPath = dlg.GetPathName();

    ExportService svc;
    CString strErr = svc.Export(m_data, strPath);

    CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

    if (strErr.IsEmpty()) {
        CString strLog, strFmt;
        strFmt.LoadString(IDS_LOG_EXPORT_OK);
        strLog.Format(strFmt, (LPCTSTR)strPath);
        sageMgr.Log(strLog);
        if (pFrame != nullptr)
            pFrame->LogMessage(strLog);
    } else {
        CString strLog, strFmt;
        strFmt.LoadString(IDS_LOG_EXPORT_FAIL);
        strLog.Format(strFmt, (LPCTSTR)strErr);
        sageMgr.Log(strLog);
        if (pFrame != nullptr)
            pFrame->LogMessage(strLog);
        AfxMessageBox(strErr, MB_OK | MB_ICONWARNING);
    }
}

void CSAGEDashDoc::OnUpdateFileExport(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_isDataLoaded);
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
