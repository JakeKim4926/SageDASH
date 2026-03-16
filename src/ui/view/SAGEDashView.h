
// SAGEDashView.h: CSAGEDashView 클래스의 인터페이스

#pragma once

#include "EnumDefine.h"
#include "MappingPanel.h"
#include "ValidationPanel.h"

class DataSheet;
class CSAGEDashDoc;

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

	void           SwitchViewMode(CenterViewMode eMode);
	CenterViewMode GetViewMode() const { return m_eViewMode; }

	MappingPanel&    GetMappingPanel()    { return m_wndMapping; }
	ValidationPanel& GetValidationPanel() { return m_wndValidation; }

public:
	virtual ~CSAGEDashView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	CEdit            m_edtSearch;
	CListCtrl        m_lstGrid;
	MappingPanel     m_wndMapping;
	ValidationPanel  m_wndValidation;
	CenterViewMode   m_eViewMode;

	void PopulateGrid(const DataSheet& sheet);
	void FilterGrid(const CString& strKeyword);
	void ClearGrid();
	void UpdateLayout(int cx, int cy);

protected:
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnEnChangeSearch();
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline CSAGEDashDoc* CSAGEDashView::GetDocument() const
	{ return reinterpret_cast<CSAGEDashDoc*>(m_pDocument); }
#endif
