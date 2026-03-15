
#pragma once

// ============================================================
// MappingPanel — Center View 매핑 모드 패널 (Phase 3 placeholder)
// ============================================================
class MappingPanel : public CWnd
{
public:
    MappingPanel();
    virtual ~MappingPanel();

    BOOL Create(CWnd* pParent, UINT nID);

protected:
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    DECLARE_MESSAGE_MAP()
};
