
// SAGEDashView.h: CSAGEDashView 클래스의 인터페이스

#pragma once

class CSAGEDashView : public CView
{
protected:
    CSAGEDashView() noexcept;
    DECLARE_DYNCREATE(CSAGEDashView)

// 특성
public:
    CSAGEDashDoc* GetDocument() const;

// 재정의
public:
    virtual void OnDraw(CDC* pDC);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

protected:
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현
public:
    virtual ~CSAGEDashView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

private:
    CListCtrl m_lstGrid;

    void PopulateGrid(const CWorksheet& sheet);
    void ClearGrid();

protected:
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnFilePrintPreview();
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline CSAGEDashDoc* CSAGEDashView::GetDocument() const
    { return reinterpret_cast<CSAGEDashDoc*>(m_pDocument); }
#endif
