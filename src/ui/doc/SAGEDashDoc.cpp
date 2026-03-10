
// SAGEDashDoc.cpp: CSAGEDashDoc 클래스의 구현

#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "SAGEDash.h"
#endif

#include "SAGEDashDoc.h"
#include "MainFrm.h"
#include "WorkbookService.h"
#include "SageException.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CSAGEDashDoc, CDocument)

BEGIN_MESSAGE_MAP(CSAGEDashDoc, CDocument)
END_MESSAGE_MAP()

CSAGEDashDoc::CSAGEDashDoc() noexcept
    : m_pWorkbook(nullptr)
{
}

CSAGEDashDoc::~CSAGEDashDoc()
{
}

BOOL CSAGEDashDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;
    return TRUE;
}

BOOL CSAGEDashDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
    DeleteContents();

    CMainFrame* pFrame = DYNAMIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
    CWorkbookService service;

    try {
        m_pWorkbook = service.LoadFromFile(lpszPathName);
    } catch (const SageException& e) {
        CString strLog;
        strLog.Format(_T("[실패] %s"), (LPCTSTR)e.Format());
        if (pFrame != nullptr)
            pFrame->LogMessage(strLog);
        return FALSE;
    }

    CWorksheet* pSheet = m_pWorkbook->GetSheet(0);
    CString strLog;
    strLog.Format(_T("[성공] %s 로드 완료 (%d행 × %d열)"),
        lpszPathName,
        pSheet->GetRowCount(),
        pSheet->GetColumnCount());
    if (pFrame != nullptr)
        pFrame->LogMessage(strLog);

    SetModifiedFlag(FALSE);
    UpdateAllViews(nullptr);
    return TRUE;
}

void CSAGEDashDoc::DeleteContents()
{
    delete m_pWorkbook;
    m_pWorkbook = nullptr;
    CDocument::DeleteContents();
}

void CSAGEDashDoc::Serialize(CArchive& ar)
{
    // CSV 파일은 OnOpenDocument에서 직접 로드하므로 Serialize 사용 안 함
    UNREFERENCED_PARAMETER(ar);
}

#ifdef SHARED_HANDLERS

void CSAGEDashDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
    dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));
}

void CSAGEDashDoc::InitializeSearchContent()
{
    SetSearchContent(_T(""));
}

void CSAGEDashDoc::SetSearchContent(const CString& value)
{
    if (value.IsEmpty()) {
        RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
    } else {
        CMFCFilterChunkValueImpl* pChunk = nullptr;
        ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
        if (pChunk != nullptr) {
            pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
            SetChunkValue(pChunk);
        }
    }
}

#endif // SHARED_HANDLERS

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
