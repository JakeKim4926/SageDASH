
// SAGEDashDoc.h: CSAGEDashDoc 클래스의 인터페이스

#pragma once

#include "TabularData.h"

class CSAGEDashDoc : public CDocument
{
protected:
	CSAGEDashDoc() noexcept;
	DECLARE_DYNCREATE(CSAGEDashDoc)

public:
	BOOL HasData() const { return m_isDataLoaded; }
	const CTabularData& GetData() const { return m_data; }

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
	CTabularData m_data;
	BOOL         m_isDataLoaded;

protected:
	DECLARE_MESSAGE_MAP()
};
