
// SAGEDashDoc.cpp: CSAGEDashDoc 클래스의 구현

#include "pch.h"
#include "framework.h"
#include <shlobj.h>
#include "SAGEDash.h"
#include "SAGEDashDoc.h"
#include "MainFrm.h"
#include "WorkbookService.h"
#include "SAGEDashView.h"
#include "ExportService.h"
#include "ProjectService.h"
#include "PipelineRunner.h"
#include "EmailActionHandler.h"
#include "ApiActionHandler.h"
#include "ApiSendDialog.h"
#include "SageException.h"
#include "SageMgr.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CSAGEDashDoc, CDocument)

BEGIN_MESSAGE_MAP(CSAGEDashDoc, CDocument)
    ON_COMMAND(ID_FILE_EXPORT,                &CSAGEDashDoc::OnFileExport)
    ON_UPDATE_COMMAND_UI(ID_FILE_EXPORT,      &CSAGEDashDoc::OnUpdateFileExport)
    ON_COMMAND(ID_FILE_SAVE_PROJECT,          &CSAGEDashDoc::OnFileSaveProject)
    ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_PROJECT,&CSAGEDashDoc::OnUpdateFileSaveProject)
    ON_COMMAND(ID_FILE_OPEN_PROJECT,          &CSAGEDashDoc::OnFileOpenProject)
    ON_COMMAND(ID_FILE_OPEN_FOLDER,              &CSAGEDashDoc::OnFileOpenFolder)
    ON_COMMAND(ID_FILE_EMAIL_ACTION,             &CSAGEDashDoc::OnFileEmailAction)
    ON_UPDATE_COMMAND_UI(ID_FILE_EMAIL_ACTION,   &CSAGEDashDoc::OnUpdateFileEmailAction)
    ON_COMMAND(ID_FILE_API_ACTION,               &CSAGEDashDoc::OnFileApiAction)
    ON_UPDATE_COMMAND_UI(ID_FILE_API_ACTION,     &CSAGEDashDoc::OnUpdateFileApiAction)
    ON_COMMAND(ID_PIPELINE_RUN,                  &CSAGEDashDoc::OnPipelineRun)
    ON_UPDATE_COMMAND_UI(ID_PIPELINE_RUN,     &CSAGEDashDoc::OnUpdatePipelineRun)
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
		pFrame->GetNavigatorPane().UpdateFileItem(lpszPathName);
		pFrame->GetNavigatorPane().ActivatePipelineItems(TRUE);
		pFrame->GetNavigatorPane().SetActiveMode(VIEW_MODE_GRID);
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
		pFrame->GetNavigatorPane().UpdateFileItem(_T(""));
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

BOOL CSAGEDashDoc::GetActiveView(CSAGEDashView*& outView) const
{
    POSITION pos = GetFirstViewPosition();
    if (pos == nullptr) {
        outView = nullptr;
        return FALSE;
    }
    outView = DYNAMIC_DOWNCAST(CSAGEDashView, GetNextView(pos));
    return outView != nullptr;
}

void CSAGEDashDoc::OnFileSaveProject()
{
    CSAGEDashView* pView = nullptr;
    if (!GetActiveView(pView))
        return;

    // 패널에서 현재 규칙 수집
    m_project.m_arrMappingRules    = pView->GetMappingPanel().GetMappingRules();
    m_project.m_arrValidationRules = pView->GetValidationPanel().GetValidationRules();

    CString strFilter;
    strFilter.LoadString(IDS_PROJ_FILE_FILTER);

    CFileDialog dlg(FALSE, _T("sagep"), nullptr,
        OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY,
        strFilter, AfxGetMainWnd());

    if (dlg.DoModal() != IDOK)
        return;

    CString strPath = dlg.GetPathName();
    ProjectService svc;
    CString strErr = svc.SaveProject(m_project, strPath);

    CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
    if (strErr.IsEmpty()) {
        CString strLog, strFmt;
        strFmt.LoadString(IDS_LOG_PROJECT_SAVED);
        strLog.Format(strFmt, (LPCTSTR)strPath);
        sageMgr.Log(strLog);
        if (pFrame != nullptr) pFrame->LogMessage(strLog);
    } else {
        CString strLog, strFmt;
        strFmt.LoadString(IDS_LOG_PROJECT_FAIL);
        strLog.Format(strFmt, (LPCTSTR)strErr);
        sageMgr.Log(strLog);
        if (pFrame != nullptr) pFrame->LogMessage(strLog);
        AfxMessageBox(strErr, MB_OK | MB_ICONWARNING);
    }
}

void CSAGEDashDoc::OnUpdateFileSaveProject(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_isDataLoaded);
}

void CSAGEDashDoc::OnFileOpenProject()
{
    CString strFilter;
    strFilter.LoadString(IDS_PROJ_FILE_FILTER);

    CFileDialog dlg(TRUE, _T("sagep"), nullptr,
        OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
        strFilter, AfxGetMainWnd());

    if (dlg.DoModal() != IDOK)
        return;

    CString strPath = dlg.GetPathName();
    AutomationProject proj;
    ProjectService svc;
    CString strErr = svc.LoadProject(strPath, proj);

    CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
    if (!strErr.IsEmpty()) {
        CString strLog, strFmt;
        strFmt.LoadString(IDS_LOG_PROJECT_FAIL);
        strLog.Format(strFmt, (LPCTSTR)strErr);
        sageMgr.Log(strLog);
        if (pFrame != nullptr) pFrame->LogMessage(strLog);
        AfxMessageBox(strErr, MB_OK | MB_ICONWARNING);
        return;
    }

    m_project = proj;

    // 패널에 규칙 주입
    CSAGEDashView* pView = nullptr;
    GetActiveView(pView);
    if (pView != nullptr) {
        if (!proj.m_arrMappingRules.empty())
            pView->GetMappingPanel().LoadRules(proj.m_arrMappingRules);
        if (!proj.m_arrValidationRules.empty())
            pView->GetValidationPanel().LoadRules(proj.m_arrValidationRules);
    }

    CString strLog, strFmt;
    strFmt.LoadString(IDS_LOG_PROJECT_LOADED);
    strLog.Format(strFmt, (LPCTSTR)strPath);
    sageMgr.Log(strLog);
    if (pFrame != nullptr) pFrame->LogMessage(strLog);
}

void CSAGEDashDoc::OnFileOpenFolder()
{
    BROWSEINFO bi = {};
    bi.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
    bi.ulFlags   = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpszTitle = _T("스캔할 폴더를 선택하세요.");

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    if (pidl == nullptr)
        return;

    TCHAR szPath[MAX_PATH] = {};
    if (!SHGetPathFromIDList(pidl, szPath)) {
        CoTaskMemFree(pidl);
        return;
    }
    CoTaskMemFree(pidl);

    CString strFolderPath = szPath;

    DeleteContents();

    CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
    WorkbookService service;

    try {
        service.LoadFromFolder(strFolderPath, m_data);
        m_isDataLoaded = TRUE;
    } catch (const SageException& e) {
        CString strLog;
        strLog.Format(_T("[실패] %s"), (LPCTSTR)e.Format());
        sageMgr.Log(strLog);
        if (pFrame != nullptr)
            pFrame->LogMessage(strLog);
        AfxMessageBox(e.GetMessage(), MB_OK | MB_ICONWARNING);
        return;
    }

    const DataSheet& sheet = m_data.GetSheet(0);
    int nFileCount = sheet.GetRowCount() - 1;  // 첫 행은 헤더

    CString strLog;
    strLog.Format(_T("[성공] 폴더 스캔 완료: %s (%d개 파일)"),
        (LPCTSTR)strFolderPath, nFileCount);
    sageMgr.Log(strLog);
    if (pFrame != nullptr) {
        pFrame->LogMessage(strLog);
        pFrame->GetPropertiesPane().SetFileInfo(strFolderPath, m_data);
        pFrame->GetNavigatorPane().UpdateFileItem(strFolderPath);
        pFrame->GetNavigatorPane().ActivatePipelineItems(TRUE);
        pFrame->GetNavigatorPane().SetActiveMode(VIEW_MODE_GRID);
    }

    SetModifiedFlag(FALSE);
    UpdateAllViews(nullptr);
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

void CSAGEDashDoc::OnPipelineRun()
{
    if (!m_isDataLoaded)
        return;

    CSAGEDashView* pView = nullptr;
    if (!GetActiveView(pView) || pView == nullptr)
        return;

    CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

    // 컨텍스트 구성
    ExecutionContext ctx;
    ctx.m_pSourceSheet       = &m_data.GetSheet(0);
    ctx.m_arrMappingRules    = pView->GetMappingPanel().GetMappingRules();
    ctx.m_arrValidationRules = pView->GetValidationPanel().GetValidationRules();
    ctx.m_strOutputPath      = m_project.m_strOutputPath;

    // 파이프라인 실행
    PipelineRunner runner;
    CString strErr = runner.Run(ctx);

    // 로그 출력
    if (pFrame != nullptr) {
        pFrame->LogMessage(_T("──── 파이프라인 실행 ────"));
        // 줄 단위로 분리해서 출력
        CString strLog = ctx.m_strLog;
        int nPos = 0;
        CString strLine;
        while (AfxExtractSubString(strLine, strLog, nPos++, _T('\n'))) {
            strLine.TrimRight(_T('\r'));
            if (!strLine.IsEmpty())
                pFrame->LogMessage(strLine);
        }
    }

    if (strErr.IsEmpty()) {
        sageMgr.Log(_T("[파이프라인] 완료"));
        if (pFrame != nullptr)
            pFrame->LogMessage(_T("[파이프라인] 완료"));
    } else {
        sageMgr.Log(_T("[파이프라인] ") + strErr);
        if (pFrame != nullptr)
            pFrame->LogMessage(_T("[파이프라인] ") + strErr);
        AfxMessageBox(strErr, MB_OK | MB_ICONWARNING);
    }
}

void CSAGEDashDoc::OnUpdatePipelineRun(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_isDataLoaded);
}

void CSAGEDashDoc::OnFileEmailAction()
{
    if (!m_isDataLoaded)
        return;

    CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

    try {
        EmailActionHandler handler;
        handler.Execute(m_data);

        CString strLog, strFmt;
        strFmt.LoadString(IDS_LOG_EMAIL_OK);
        strLog.Format(strFmt, (LPCTSTR)m_data.m_strFilePath);
        sageMgr.Log(strLog);
        if (pFrame != nullptr)
            pFrame->LogMessage(strLog);
    } catch (const SageException& e) {
        CString strLog, strFmt;
        strFmt.LoadString(IDS_LOG_EMAIL_FAIL);
        strLog.Format(strFmt, (LPCTSTR)e.GetMessage());
        sageMgr.Log(strLog);
        if (pFrame != nullptr)
            pFrame->LogMessage(strLog);
        AfxMessageBox(e.GetMessage(), MB_OK | MB_ICONWARNING);
    } catch (...) {
        CString strMsg = _T("알 수 없는 오류가 발생했습니다.");
        if (pFrame != nullptr)
            pFrame->LogMessage(strMsg);
        AfxMessageBox(strMsg, MB_OK | MB_ICONWARNING);
    }
}

void CSAGEDashDoc::OnUpdateFileEmailAction(CCmdUI* pCmdUI)
{
    pCmdUI->Enable(m_isDataLoaded);
}

void CSAGEDashDoc::OnFileApiAction()
{
    if (!m_isDataLoaded)
        return;

    ApiSendDialog dlg(AfxGetMainWnd());
    if (dlg.DoModal() != IDOK)
        return;

    CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());

    try {
        ApiActionHandler handler(dlg.m_strUrl, dlg.m_strMethod);
        handler.Execute(m_data);

        CString strLog, strFmt;
        strFmt.LoadString(IDS_LOG_API_OK);
        strLog.Format(strFmt, (LPCTSTR)dlg.m_strUrl);
        sageMgr.Log(strLog);
        if (pFrame != nullptr)
            pFrame->LogMessage(strLog);
    } catch (const SageException& e) {
        CString strLog, strFmt;
        strFmt.LoadString(IDS_LOG_API_FAIL);
        strLog.Format(strFmt, (LPCTSTR)e.GetMessage());
        sageMgr.Log(strLog);
        if (pFrame != nullptr)
            pFrame->LogMessage(strLog);
        AfxMessageBox(e.GetMessage(), MB_OK | MB_ICONWARNING);
    } catch (...) {
        CString strMsg = _T("알 수 없는 오류가 발생했습니다.");
        if (pFrame != nullptr)
            pFrame->LogMessage(strMsg);
        AfxMessageBox(strMsg, MB_OK | MB_ICONWARNING);
    }
}

void CSAGEDashDoc::OnUpdateFileApiAction(CCmdUI* pCmdUI)
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
