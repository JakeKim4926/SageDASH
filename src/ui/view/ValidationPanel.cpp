
#include "pch.h"
#include "framework.h"
#include "ValidationPanel.h"
#include "ValidationService.h"
#include "Define.h"
#include "Resource.h"


BEGIN_MESSAGE_MAP(ValidationPanel, CWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_BN_CLICKED(IDC_VAL_BTN_ADD,         OnBnClickedAdd)
    ON_BN_CLICKED(IDC_VAL_BTN_DELETE,      OnBnClickedDelete)
    ON_BN_CLICKED(IDC_VAL_BTN_CLEAR_RULES, OnBnClickedClearRules)
    ON_BN_CLICKED(IDC_VAL_BTN_RUN,         OnBnClickedRun)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_VAL_LIST_RULES,   OnLvnItemChangedRules)
    ON_NOTIFY(NM_CUSTOMDRAW,   IDC_VAL_LIST_RESULTS, OnCustomDrawResults)
END_MESSAGE_MAP()

ValidationPanel::ValidationPanel()
    : m_bHasResult(FALSE)
    , m_pDataSheet(nullptr)
{
}

ValidationPanel::~ValidationPanel()
{
}

BOOL ValidationPanel::Create(CWnd* pParent, UINT nID)
{
    CRect rectDummy;
    rectDummy.SetRectEmpty();
    return CWnd::Create(nullptr, nullptr,
        WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        rectDummy, pParent, nID);
}

int ValidationPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    CreateControls();
    return 0;
}

void ValidationPanel::CreateControls()
{
    CRect rectDummy;
    rectDummy.SetRectEmpty();

    // 컬럼 콤보
    m_cmbColumn.Create(
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        rectDummy, this, IDC_VAL_COMBO_COLUMN);
    m_cmbColumn.SetItemHeight(-1, VAL_COMBO_ITEM_H);

    // 타입 콤보
    m_cmbType.Create(
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        rectDummy, this, IDC_VAL_COMBO_TYPE);
    m_cmbType.SetItemHeight(-1, VAL_COMBO_ITEM_H);

    CString strRequired, strFormat;
    strRequired.LoadString(IDS_VIEW_VAL_TYPE_REQUIRED);
    strFormat.LoadString(IDS_VIEW_VAL_TYPE_FORMAT);
    m_cmbType.AddString(strRequired);
    m_cmbType.AddString(strFormat);
    m_cmbType.SetCurSel(0);

    // 심각도 콤보
    m_cmbSeverity.Create(
        WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL,
        rectDummy, this, IDC_VAL_COMBO_SEVERITY);
    m_cmbSeverity.SetItemHeight(-1, VAL_COMBO_ITEM_H);

    CString strError, strWarning;
    strError.LoadString(IDS_VIEW_VAL_SEV_ERROR);
    strWarning.LoadString(IDS_VIEW_VAL_SEV_WARNING);
    m_cmbSeverity.AddString(strError);
    m_cmbSeverity.AddString(strWarning);
    m_cmbSeverity.SetCurSel(0);

    // 패턴 에디트
    m_edtPattern.Create(
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
        rectDummy, this, IDC_VAL_EDIT_PATTERN);

    // 추가/삭제/전체삭제 버튼
    CString strAdd, strDelete, strClear;
    strAdd.LoadString(IDS_VIEW_VAL_BTN_ADD);
    strDelete.LoadString(IDS_VIEW_VAL_BTN_DELETE);
    strClear.LoadString(IDS_VIEW_VAL_BTN_CLEAR_RULES);

    m_btnAdd.Create(strAdd, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        rectDummy, this, IDC_VAL_BTN_ADD);
    m_btnDelete.Create(strDelete, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        rectDummy, this, IDC_VAL_BTN_DELETE);
    m_btnClearRules.Create(strClear, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        rectDummy, this, IDC_VAL_BTN_CLEAR_RULES);

    // 규칙 리스트
    m_lstRules.Create(
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER | LVS_SINGLESEL,
        rectDummy, this, IDC_VAL_LIST_RULES);
    m_lstRules.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    LVCOLUMN lvc = {};
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;

    CString str;
    str.LoadString(IDS_VIEW_VAL_COL_NUM);
    lvc.fmt = LVCFMT_RIGHT; lvc.cx = VAL_COL_NUM_W;      lvc.pszText = (LPTSTR)(LPCTSTR)str;
    m_lstRules.InsertColumn(0, &lvc);

    str.LoadString(IDS_VIEW_VAL_COL_COLUMN);
    lvc.fmt = LVCFMT_LEFT;  lvc.cx = VAL_COL_COLUMN_W;   lvc.pszText = (LPTSTR)(LPCTSTR)str;
    m_lstRules.InsertColumn(1, &lvc);

    str.LoadString(IDS_VIEW_VAL_COL_TYPE);
    lvc.fmt = LVCFMT_LEFT;  lvc.cx = VAL_COL_TYPE_W;     lvc.pszText = (LPTSTR)(LPCTSTR)str;
    m_lstRules.InsertColumn(2, &lvc);

    str.LoadString(IDS_VIEW_VAL_COL_SEVERITY);
    lvc.fmt = LVCFMT_LEFT;  lvc.cx = VAL_COL_SEVERITY_W; lvc.pszText = (LPTSTR)(LPCTSTR)str;
    m_lstRules.InsertColumn(3, &lvc);

    str.LoadString(IDS_VIEW_VAL_COL_PATTERN);
    lvc.fmt = LVCFMT_LEFT;  lvc.cx = VAL_COL_PATTERN_W;  lvc.pszText = (LPTSTR)(LPCTSTR)str;
    m_lstRules.InsertColumn(4, &lvc);

    // 검증 실행 버튼
    CString strRun;
    strRun.LoadString(IDS_VIEW_VAL_BTN_RUN);
    m_btnRun.Create(strRun, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        rectDummy, this, IDC_VAL_BTN_RUN);

    // 결과 리스트
    m_lstResults.Create(
        WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER,
        rectDummy, this, IDC_VAL_LIST_RESULTS);
    m_lstResults.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    str.LoadString(IDS_VIEW_VAL_COL_NUM);
    lvc.fmt = LVCFMT_RIGHT; lvc.cx = VAL_COL_NUM_W;      lvc.pszText = (LPTSTR)(LPCTSTR)str;
    m_lstResults.InsertColumn(0, &lvc);

    str.LoadString(IDS_VIEW_VAL_RES_COL_ROW);
    lvc.fmt = LVCFMT_RIGHT; lvc.cx = VAL_RES_COL_ROW_W;  lvc.pszText = (LPTSTR)(LPCTSTR)str;
    m_lstResults.InsertColumn(1, &lvc);

    str.LoadString(IDS_VIEW_VAL_COL_COLUMN);
    lvc.fmt = LVCFMT_LEFT;  lvc.cx = VAL_RES_COL_COL_W;  lvc.pszText = (LPTSTR)(LPCTSTR)str;
    m_lstResults.InsertColumn(2, &lvc);

    str.LoadString(IDS_VIEW_VAL_RES_COL_MSG);
    lvc.fmt = LVCFMT_LEFT;  lvc.cx = VAL_RES_COL_MSG_W;  lvc.pszText = (LPTSTR)(LPCTSTR)str;
    m_lstResults.InsertColumn(3, &lvc);

    str.LoadString(IDS_VIEW_VAL_COL_SEVERITY);
    lvc.fmt = LVCFMT_LEFT;  lvc.cx = VAL_COL_SEVERITY_W; lvc.pszText = (LPTSTR)(LPCTSTR)str;
    m_lstResults.InsertColumn(4, &lvc);

    UpdateButtonStates();
}

void ValidationPanel::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    if (m_cmbColumn.GetSafeHwnd() == nullptr)
        return;

    UpdateLayout(cx, cy);
}

void ValidationPanel::UpdateLayout(int cx, int cy)
{
    int nY = VAL_HEADER_H + VAL_ROW_GAP;

    // Controls row: [컬럼 150][타입 72][심각도 60][패턴 ~][추가 48]
    {
        int nColW  = 150;
        int nTypeW = VAL_COL_TYPE_W;
        int nSevW  = VAL_COL_SEVERITY_W;
        int nPatW  = cx - VAL_PADDING * 2 - nColW - VAL_ROW_GAP - nTypeW - VAL_ROW_GAP
                     - nSevW - VAL_ROW_GAP - VAL_BTN_ADD_W - VAL_ROW_GAP;
        if (nPatW < VAL_MIN_PATTERN_W) nPatW = VAL_MIN_PATTERN_W;

        int xPos = VAL_PADDING;
        m_cmbColumn.SetWindowPos(nullptr, xPos, nY, nColW, COMBO_DROPDOWN_H, SWP_NOZORDER | SWP_NOACTIVATE);
        xPos += nColW + VAL_ROW_GAP;

        m_cmbType.SetWindowPos(nullptr, xPos, nY, nTypeW, COMBO_DROPDOWN_H, SWP_NOZORDER | SWP_NOACTIVATE);
        xPos += nTypeW + VAL_ROW_GAP;

        m_cmbSeverity.SetWindowPos(nullptr, xPos, nY, nSevW, COMBO_DROPDOWN_H, SWP_NOZORDER | SWP_NOACTIVATE);
        xPos += nSevW + VAL_ROW_GAP;

        m_edtPattern.SetWindowPos(nullptr, xPos, nY, nPatW, VAL_CONTROLS_H, SWP_NOZORDER | SWP_NOACTIVATE);
        xPos += nPatW + VAL_ROW_GAP;

        m_btnAdd.SetWindowPos(nullptr, xPos, nY, VAL_BTN_ADD_W, VAL_CONTROLS_H, SWP_NOZORDER | SWP_NOACTIVATE);
    }
    nY += VAL_CONTROLS_H + VAL_ROW_GAP;

    // Buttons row: [삭제 52][전체삭제 70]
    {
        int xPos = VAL_PADDING;
        m_btnDelete.SetWindowPos(nullptr, xPos, nY, VAL_BTN_DELETE_W, VAL_BUTTONS_H, SWP_NOZORDER | SWP_NOACTIVATE);
        xPos += VAL_BTN_DELETE_W + VAL_ROW_GAP;
        m_btnClearRules.SetWindowPos(nullptr, xPos, nY, VAL_BTN_CLEAR_W, VAL_BUTTONS_H, SWP_NOZORDER | SWP_NOACTIVATE);
    }
    nY += VAL_BUTTONS_H + VAL_ROW_GAP;

    // Rules list
    int nRulesH = VAL_RULES_LIST_H;
    m_lstRules.SetWindowPos(nullptr, 0, nY, cx, nRulesH, SWP_NOZORDER | SWP_NOACTIVATE);
    nY += nRulesH + VAL_ROW_GAP;

    // Run row: [검증 실행 80] + summary text (drawn by OnPaint)
    m_btnRun.SetWindowPos(nullptr, VAL_PADDING, nY + (VAL_RUN_H - VAL_CONTROLS_H) / 2,
        VAL_BTN_RUN_W, VAL_CONTROLS_H, SWP_NOZORDER | SWP_NOACTIVATE);
    nY += VAL_RUN_H + VAL_ROW_GAP;

    // Results list: rest
    int nResultsH = cy - nY;
    if (nResultsH < 0) nResultsH = 0;
    m_lstResults.SetWindowPos(nullptr, 0, nY, cx, nResultsH, SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL ValidationPanel::OnEraseBkgnd(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);
    pDC->FillSolidRect(&rect, COLOR_WHITE);
    return TRUE;
}

void ValidationPanel::OnPaint()
{
    CPaintDC dc(this);

    CRect rcClient;
    GetClientRect(&rcClient);
    dc.FillSolidRect(&rcClient, COLOR_WHITE);

    CFont* pOldFont = (CFont*)dc.SelectStockObject(DEFAULT_GUI_FONT);
    dc.SetBkMode(TRANSPARENT);

    // 헤더 영역
    {
        CRect rcHeader(0, 0, rcClient.right, VAL_HEADER_H);
        dc.FillSolidRect(&rcHeader, COLOR_SURFACE);

        CString strTitle;
        strTitle.LoadString(IDS_VIEW_VAL_TITLE);
        dc.SetTextColor(COLOR_TEXT);
        CRect rcText = rcHeader;
        rcText.left += VAL_PADDING;
        dc.DrawText(strTitle, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

        // 하단 구분선
        dc.FillSolidRect(0, VAL_HEADER_H - 1, rcClient.right, 1, COLOR_BORDER);
    }

    // 규칙 리스트와 실행 버튼 사이 구분선 및 결과 섹션 헤더
    {
        int nRunY = VAL_HEADER_H + VAL_ROW_GAP
                  + VAL_CONTROLS_H + VAL_ROW_GAP
                  + VAL_BUTTONS_H + VAL_ROW_GAP
                  + VAL_RULES_LIST_H + VAL_ROW_GAP;

        // 실행 행 배경
        COLORREF clrRunBg = COLOR_SURFACE;
        if (m_bHasResult) {
            if (m_lastResult.HasErrors())
                clrRunBg = COLOR_ERROR_BG;
            else if (m_lastResult.HasWarnings())
                clrRunBg = COLOR_WARNING_BG;
            else
                clrRunBg = COLOR_SUCCESS_BG;
        }
        CRect rcRun(0, nRunY, rcClient.right, nRunY + VAL_RUN_H);
        dc.FillSolidRect(&rcRun, clrRunBg);
        dc.FillSolidRect(0, nRunY, rcClient.right, 1, COLOR_BORDER);
        dc.FillSolidRect(0, nRunY + VAL_RUN_H - 1, rcClient.right, 1, COLOR_BORDER);

        // 요약 텍스트 (버튼 오른쪽)
        if (m_bHasResult) {
            CString strSummary;
            if (!m_lastResult.HasErrors() && !m_lastResult.HasWarnings()) {
                strSummary.LoadString(IDS_VIEW_VAL_STATUS_OK);
                dc.SetTextColor(COLOR_SUCCESS);
            } else {
                CString strFmt;
                strFmt.LoadString(IDS_VIEW_VAL_STATUS_FMT);
                strSummary.Format(strFmt,
                    m_lastResult.GetErrorCount(),
                    m_lastResult.GetWarningCount());
                dc.SetTextColor(m_lastResult.HasErrors() ? COLOR_ERROR : COLOR_WARNING);
            }

            CRect rcSummary(VAL_PADDING + VAL_BTN_RUN_W + VAL_PADDING, nRunY,
                            rcClient.right - VAL_PADDING, nRunY + VAL_RUN_H);
            dc.DrawText(strSummary, &rcSummary, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
        }
    }

    dc.SelectObject(pOldFont);
}

void ValidationPanel::SetSourceColumns(const std::vector<CString>& arrColumns)
{
    m_arrSourceColumns = arrColumns;

    if (m_cmbColumn.GetSafeHwnd() == nullptr)
        return;

    m_cmbColumn.ResetContent();
    for (int i = 0; i < (int)arrColumns.size(); i++) {
        m_cmbColumn.AddString(arrColumns[i]);
    }
    if (!arrColumns.empty())
        m_cmbColumn.SetCurSel(0);
}

void ValidationPanel::SetDataSheet(const DataSheet* pSheet)
{
    m_pDataSheet = pSheet;
}

void ValidationPanel::LoadRules(const std::vector<ValidationRule>& arrRules)
{
    m_arrRules = arrRules;
    m_lastResult.Clear();
    m_bHasResult = FALSE;
    RebuildRulesList();
    if (m_lstResults.GetSafeHwnd() != nullptr)
        m_lstResults.DeleteAllItems();
    Invalidate();
    UpdateButtonStates();
}

void ValidationPanel::ClearAll()
{
    m_arrSourceColumns.clear();
    m_arrRules.clear();
    m_lastResult.Clear();
    m_bHasResult = FALSE;
    m_pDataSheet  = nullptr;

    if (m_cmbColumn.GetSafeHwnd() != nullptr)
        m_cmbColumn.ResetContent();
    if (m_lstRules.GetSafeHwnd() != nullptr)
        m_lstRules.DeleteAllItems();
    if (m_lstResults.GetSafeHwnd() != nullptr)
        m_lstResults.DeleteAllItems();

    Invalidate();
    UpdateButtonStates();
}

void ValidationPanel::AddRule()
{
    int nColSel = m_cmbColumn.GetCurSel();
    if (nColSel < 0)
        return;

    CString strColumn;
    m_cmbColumn.GetLBText(nColSel, strColumn);

    int nTypeSel = m_cmbType.GetCurSel();
    if (nTypeSel < 0) nTypeSel = 0;
    ValidationType eType = (nTypeSel == 0) ? VALIDATION_REQUIRED : VALIDATION_FORMAT;

    int nSevSel = m_cmbSeverity.GetCurSel();
    if (nSevSel < 0) nSevSel = 0;
    ValidationSeverity eSev = (nSevSel == 0) ? VALIDATION_SEVERITY_ERROR : VALIDATION_SEVERITY_WARNING;

    CString strPattern;
    m_edtPattern.GetWindowText(strPattern);
    strPattern.Trim();

    ValidationRule rule(strColumn, eType, eSev);
    rule.SetPattern(strPattern);
    m_arrRules.push_back(rule);

    RebuildRulesList();
    UpdateButtonStates();

    m_edtPattern.SetWindowText(_T(""));
}

void ValidationPanel::DeleteSelectedRule()
{
    int nSel = m_lstRules.GetNextItem(-1, LVNI_SELECTED);
    if (nSel < 0 || nSel >= (int)m_arrRules.size())
        return;

    m_arrRules.erase(m_arrRules.begin() + nSel);
    RebuildRulesList();
    UpdateButtonStates();
}

void ValidationPanel::RunValidation()
{
    if (m_pDataSheet == nullptr || m_arrRules.empty())
        return;

    ValidationService svc;
    m_lastResult = svc.Validate(*m_pDataSheet, m_arrRules);
    m_bHasResult = TRUE;

    RebuildResultsList();
    Invalidate();
}

void ValidationPanel::RebuildRulesList()
{
    m_lstRules.DeleteAllItems();

    CString strRequired, strFormat, strError, strWarning;
    strRequired.LoadString(IDS_VIEW_VAL_TYPE_REQUIRED);
    strFormat.LoadString(IDS_VIEW_VAL_TYPE_FORMAT);
    strError.LoadString(IDS_VIEW_VAL_SEV_ERROR);
    strWarning.LoadString(IDS_VIEW_VAL_SEV_WARNING);

    for (int i = 0; i < (int)m_arrRules.size(); i++) {
        const ValidationRule& rule = m_arrRules[i];

        CString strNum;
        strNum.Format(_T("%d"), i + 1);

        LVITEM lvi = {};
        lvi.mask    = LVIF_TEXT;
        lvi.iItem   = i;
        lvi.pszText = (LPTSTR)(LPCTSTR)strNum;
        int nIdx = m_lstRules.InsertItem(&lvi);

        m_lstRules.SetItemText(nIdx, 1, rule.GetColumnName());
        m_lstRules.SetItemText(nIdx, 2,
            (rule.GetType() == VALIDATION_REQUIRED) ? strRequired : strFormat);
        m_lstRules.SetItemText(nIdx, 3,
            (rule.GetSeverity() == VALIDATION_SEVERITY_ERROR) ? strError : strWarning);
        m_lstRules.SetItemText(nIdx, 4, rule.GetPattern());
    }
}

void ValidationPanel::RebuildResultsList()
{
    m_lstResults.DeleteAllItems();

    CString strError, strWarning;
    strError.LoadString(IDS_VIEW_VAL_SEV_ERROR);
    strWarning.LoadString(IDS_VIEW_VAL_SEV_WARNING);

    const std::vector<ValidationIssue>& issues = m_lastResult.GetIssues();
    for (int i = 0; i < (int)issues.size(); i++) {
        const ValidationIssue& issue = issues[i];

        CString strNum;
        strNum.Format(_T("%d"), i + 1);

        LVITEM lvi = {};
        lvi.mask    = LVIF_TEXT;
        lvi.iItem   = i;
        lvi.pszText = (LPTSTR)(LPCTSTR)strNum;
        int nIdx = m_lstResults.InsertItem(&lvi);

        CString strRow;
        strRow.Format(_T("%d"), issue.GetRow());
        m_lstResults.SetItemText(nIdx, 1, strRow);
        m_lstResults.SetItemText(nIdx, 2, issue.GetColumn());
        m_lstResults.SetItemText(nIdx, 3, issue.GetMessage());
        m_lstResults.SetItemText(nIdx, 4,
            issue.IsError() ? strError : strWarning);
    }
}

void ValidationPanel::UpdateButtonStates()
{
    if (m_btnDelete.GetSafeHwnd() == nullptr)
        return;

    BOOL bHasSel   = (m_lstRules.GetNextItem(-1, LVNI_SELECTED) >= 0);
    BOOL bHasRules = !m_arrRules.empty();
    BOOL bCanRun   = bHasRules && (m_pDataSheet != nullptr);

    m_btnDelete.EnableWindow(bHasSel);
    m_btnClearRules.EnableWindow(bHasRules);
    m_btnRun.EnableWindow(bCanRun);
}

void ValidationPanel::OnBnClickedAdd()
{
    AddRule();
}

void ValidationPanel::OnBnClickedDelete()
{
    DeleteSelectedRule();
}

void ValidationPanel::OnBnClickedClearRules()
{
    m_arrRules.clear();
    RebuildRulesList();
    m_lastResult.Clear();
    m_bHasResult = FALSE;
    if (m_lstResults.GetSafeHwnd() != nullptr)
        m_lstResults.DeleteAllItems();
    Invalidate();
    UpdateButtonStates();
}

void ValidationPanel::OnBnClickedRun()
{
    RunValidation();
}

void ValidationPanel::OnLvnItemChangedRules(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
    UpdateButtonStates();
    *pResult = 0;
}

void ValidationPanel::OnCustomDrawResults(NMHDR* pNMHDR, LRESULT* pResult)
{
    NMLVCUSTOMDRAW* pCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
    *pResult = CDRF_DODEFAULT;

    if (pCD->nmcd.dwDrawStage == CDDS_PREPAINT) {
        *pResult = CDRF_NOTIFYITEMDRAW;
        return;
    }

    if (pCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT) {
        int nItem = (int)pCD->nmcd.dwItemSpec;
        const std::vector<ValidationIssue>& issues = m_lastResult.GetIssues();
        if (nItem >= 0 && nItem < (int)issues.size()) {
            if (issues[nItem].IsError()) {
                pCD->clrTextBk = COLOR_ERROR_BG;
                pCD->clrText   = COLOR_TEXT;
            } else {
                pCD->clrTextBk = COLOR_WARNING_BG;
                pCD->clrText   = COLOR_TEXT;
            }
        }
        *pResult = CDRF_NEWFONT;
    }
}
