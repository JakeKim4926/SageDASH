
// SAGEDashView.h: CSAGEDashView 클래스의 인터페이스

#pragma once

class DataSheet;

class CSAGEDashView : public CView
{
protected:
	CSAGEDashView() noexcept;
	DECLARE_DYNCREATE(CSAGEDashView)

public:
	CSAGEDashDoc* GetDocument() const;

public:
	virtual void OnDraw(CDC* pDC);
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

public:
	virtual ~CSAGEDashView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	CListCtrl m_lstGrid;

	void PopulateGrid(const DataSheet& sheet);
	void ClearGrid();

protected:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline CSAGEDashDoc* CSAGEDashView::GetDocument() const
	{ return reinterpret_cast<CSAGEDashDoc*>(m_pDocument); }
#endif
