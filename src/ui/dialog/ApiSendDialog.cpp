
#include "pch.h"
#include "framework.h"
#include "ApiSendDialog.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(ApiSendDialog, CDialogEx)
END_MESSAGE_MAP()

ApiSendDialog::ApiSendDialog(CWnd* pParent)
    : CDialogEx(IDD_API_SEND, pParent)
    , m_strMethod(_T("POST"))
{
}

void ApiSendDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_API_URL, m_strUrl);
}

BOOL ApiSendDialog::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_METHOD);
    if (pCombo != nullptr) {
        pCombo->AddString(_T("POST"));
        pCombo->AddString(_T("PUT"));
        pCombo->SetCurSel(0);
    }

    return TRUE;
}

void ApiSendDialog::OnOK()
{
    UpdateData(TRUE);

    m_strUrl.Trim();
    if (m_strUrl.IsEmpty()) {
        AfxMessageBox(_T("URL을 입력하세요."), MB_OK | MB_ICONWARNING);
        GetDlgItem(IDC_EDIT_API_URL)->SetFocus();
        return;
    }

    CComboBox* pCombo = (CComboBox*)GetDlgItem(IDC_COMBO_METHOD);
    if (pCombo != nullptr) {
        int nSel = pCombo->GetCurSel();
        if (nSel >= 0)
            pCombo->GetLBText(nSel, m_strMethod);
    }

    CDialogEx::OnOK();
}
