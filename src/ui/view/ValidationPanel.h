
#pragma once

// ============================================================
// ValidationPanel — Center View 검증 모드 패널 (Phase 3 placeholder)
// ============================================================
class ValidationPanel : public CWnd
{
public:
    ValidationPanel();
    virtual ~ValidationPanel();

    BOOL Create(CWnd* pParent, UINT nID);

protected:
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    DECLARE_MESSAGE_MAP()
};
