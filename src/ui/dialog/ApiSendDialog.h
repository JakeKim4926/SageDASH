
#pragma once

// ============================================================
// ApiSendDialog — API 전송 액션 설정 다이얼로그
// ============================================================
class ApiSendDialog : public CDialogEx
{
public:
    explicit ApiSendDialog(CWnd* pParent = nullptr);

    CString m_strUrl;
    CString m_strMethod;   // "POST" or "PUT"

protected:
    virtual void DoDataExchange(CDataExchange* pDX) override;
    virtual BOOL OnInitDialog() override;
    virtual void OnOK() override;

    DECLARE_MESSAGE_MAP()
};
