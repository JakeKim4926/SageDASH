
// SAGEDashDoc.h: CSAGEDashDoc 클래스의 인터페이스

#pragma once

#include "TabularData.h"
#include "AutomationProject.h"

class CSAGEDashView;

class CSAGEDashDoc : public CDocument
{
protected:
	CSAGEDashDoc() noexcept;
	DECLARE_DYNCREATE(CSAGEDashDoc)

public:
	BOOL HasData() const { return m_isDataLoaded; }
	const TabularData& GetData() const { return m_data; }

public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	virtual void Serialize(CArchive& ar);
	virtual void ReportSaveLoadException(LPCTSTR lpszPathName, CException* e, BOOL bSaving, UINT nIDPDefault);

public:
	virtual ~CSAGEDashDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	TabularData       m_data;
	BOOL              m_isDataLoaded;
	AutomationProject m_project;

	BOOL GetActiveView(CSAGEDashView*& outView) const;

protected:
	afx_msg void OnFileExport();
	afx_msg void OnUpdateFileExport(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveProject();
	afx_msg void OnUpdateFileSaveProject(CCmdUI* pCmdUI);
	afx_msg void OnFileOpenProject();
	afx_msg void OnFileOpenFolder();
	afx_msg void OnFileEmailAction();
	afx_msg void OnUpdateFileEmailAction(CCmdUI* pCmdUI);
	afx_msg void OnFileApiAction();
	afx_msg void OnUpdateFileApiAction(CCmdUI* pCmdUI);
	afx_msg void OnFileFtpAction();
	afx_msg void OnUpdateFileFtpAction(CCmdUI* pCmdUI);
	afx_msg void OnAutomationBatch();
	afx_msg void OnUpdateAutomationBatch(CCmdUI* pCmdUI);
	afx_msg void OnPipelineRun();
	afx_msg void OnUpdatePipelineRun(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};
