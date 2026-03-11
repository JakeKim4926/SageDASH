
// SAGEDashDoc.h: CSAGEDashDoc 클래스의 인터페이스

#pragma once

#include "Workbook.h"

class CSAGEDashDoc : public CDocument
{
protected:
    CSAGEDashDoc() noexcept;
    DECLARE_DYNCREATE(CSAGEDashDoc)

// 특성
public:
    BOOL HasWorkbook() const { return m_isWorkbookLoaded; }
    const CWorkbook& GetWorkbook() const { return m_workbook; }

// 재정의
public:
    virtual BOOL OnNewDocument();
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
    virtual void DeleteContents();
    virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
    virtual void InitializeSearchContent();
    virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif

// 구현
public:
    virtual ~CSAGEDashDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

private:
    CWorkbook   m_workbook;
    BOOL        m_isWorkbookLoaded;

protected:
    DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
    void SetSearchContent(const CString& value);
#endif
};
