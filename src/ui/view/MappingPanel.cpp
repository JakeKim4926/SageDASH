
#include "pch.h"
#include "framework.h"
#include "MappingPanel.h"
#include "Define.h"
#include "Resource.h"

#define IDC_MAP_COMBO_SOURCE    200
#define IDC_MAP_EDIT_TARGET     201
#define IDC_MAP_CHK_REQUIRED    202
#define IDC_MAP_BTN_ADD         203
#define IDC_MAP_BTN_DELETE      204
#define IDC_MAP_BTN_AUTOMAP     205
#define IDC_MAP_BTN_CLEAR       206
#define IDC_MAP_LIST_RULES      207

constexpr int MAP_HEADER_H      = 28;
constexpr int MAP_CONTROLS_H    = 24;
constexpr int MAP_BUTTONS_H     = 24;
constexpr int MAP_PADDING       = 8;
constexpr int MAP_ROW_GAP       = 4;
constexpr int MAP_COMBO_W       = 180;
constexpr int MAP_EDIT_W        = 180;
constexpr int MAP_CHK_W         = 48;
constexpr int MAP_BTN_ADD_W     = 48;
constexpr int MAP_BTN_DELETE_W  = 52;
constexpr int MAP_BTN_AUTOMAP_W = 80;
constexpr int MAP_BTN_CLEAR_W   = 52;
constexpr int MAP_BTN_GAP       = 4;

BEGIN_MESSAGE_MAP(MappingPanel, CWnd)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_BN_CLICKED(IDC_MAP_BTN_ADD,     &MappingPanel::OnBnClickedAdd)
    ON_BN_CLICKED(IDC_MAP_BTN_DELETE,  &MappingPanel::OnBnClickedDelete)
    ON_BN_CLICKED(IDC_MAP_BTN_AUTOMAP, &MappingPanel::OnBnClickedAutoMap)
    ON_BN_CLICKED(IDC_MAP_BTN_CLEAR,   &MappingPanel::OnBnClickedClear)
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_MAP_LIST_RULES, &MappingPanel::OnLvnItemChangedRules)
END_MESSAGE_MAP()

MappingPanel::MappingPanel()
{
}

MappingPanel::~MappingPanel()
{
}

BOOL MappingPanel::Create(CWnd* pParent, UINT nID)
{
    CRect rectDummy;
    rectDummy.SetRectEmpty();
    return CWnd::Create(nullptr, nullptr,
        WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        rectDummy, pParent, nID);
}

int MappingPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    CreateControls();
    return 0;
}

void MappingPanel::CreateControls()
{
    CRect rectDummy;
    rectDummy.SetRectEmpty();

    // Source ComboBox
    if (!m_cmbSource.Create(
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST,
            rectDummy, this, IDC_MAP_COMBO_SOURCE)) {
        TRACE0("Source ComboBox를 만들지 못했습니다.\n");
    }

    // Target Edit
    if (!m_edtTarget.Create(
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL,
            rectDummy, this, IDC_MAP_EDIT_TARGET)) {
        TRACE0("Target Edit를 만들지 못했습니다.\n");
    }
    CString strHint;
    strHint.LoadString(IDS_VIEW_MAPPING_TARGET_HINT);
    m_edtTarget.SendMessage(EM_SETCUEBANNER, FALSE, (LPARAM)(LPCTSTR)strHint);

    // Required Checkbox
    CString strRequired;
    strRequired.LoadString(IDS_VIEW_MAPPING_COL_REQUIRED);
    if (!m_chkRequired.Create(strRequired,
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
            rectDummy, this, IDC_MAP_CHK_REQUIRED)) {
        TRACE0("Required 체크박스를 만들지 못했습니다.\n");
    }

    // Add button
    CString strAdd;
    strAdd.LoadString(IDS_VIEW_MAPPING_BTN_ADD);
    if (!m_btnAdd.Create(strAdd,
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
            rectDummy, this, IDC_MAP_BTN_ADD)) {
        TRACE0("추가 버튼을 만들지 못했습니다.\n");
    }

    // Delete button
    CString strDelete;
    strDelete.LoadString(IDS_VIEW_MAPPING_BTN_DELETE);
    if (!m_btnDelete.Create(strDelete,
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
            rectDummy, this, IDC_MAP_BTN_DELETE)) {
        TRACE0("삭제 버튼을 만들지 못했습니다.\n");
    }

    // AutoMap button
    CString strAutoMap;
    strAutoMap.LoadString(IDS_VIEW_MAPPING_BTN_AUTOMAP);
    if (!m_btnAutoMap.Create(strAutoMap,
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
            rectDummy, this, IDC_MAP_BTN_AUTOMAP)) {
        TRACE0("자동 매핑 버튼을 만들지 못했습니다.\n");
    }

    // Clear button
    CString strClear;
    strClear.LoadString(IDS_VIEW_MAPPING_BTN_CLEAR);
    if (!m_btnClear.Create(strClear,
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
            rectDummy, this, IDC_MAP_BTN_CLEAR)) {
        TRACE0("초기화 버튼을 만들지 못했습니다.\n");
    }

    // Rules ListCtrl
    if (!m_lstRules.Create(
            WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_NOSORTHEADER,
            rectDummy, this, IDC_MAP_LIST_RULES)) {
        TRACE0("규칙 목록을 만들지 못했습니다.\n");
    }
    m_lstRules.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

    // List columns: # | Source | Target | 필수
    LVCOLUMN lvc = {};
    CString strCol;

    strCol.LoadString(IDS_VIEW_MAPPING_COL_NUM);
    lvc.mask    = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
    lvc.fmt     = LVCFMT_RIGHT;
    lvc.cx      = 40;
    lvc.pszText = (LPTSTR)(LPCTSTR)strCol;
    m_lstRules.InsertColumn(0, &lvc);

    strCol.LoadString(IDS_VIEW_MAPPING_COL_SOURCE);
    lvc.fmt     = LVCFMT_LEFT;
    lvc.cx      = 180;
    lvc.pszText = (LPTSTR)(LPCTSTR)strCol;
    m_lstRules.InsertColumn(1, &lvc);

    strCol.LoadString(IDS_VIEW_MAPPING_COL_TARGET);
    lvc.cx      = 180;
    lvc.pszText = (LPTSTR)(LPCTSTR)strCol;
    m_lstRules.InsertColumn(2, &lvc);

    strCol.LoadString(IDS_VIEW_MAPPING_COL_REQUIRED);
    lvc.fmt     = LVCFMT_CENTER;
    lvc.cx      = 50;
    lvc.pszText = (LPTSTR)(LPCTSTR)strCol;
    m_lstRules.InsertColumn(3, &lvc);

    UpdateButtonStates();
}

void MappingPanel::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    if (m_cmbSource.GetSafeHwnd() == nullptr)
        return;

    UpdateLayout(cx, cy);
}

void MappingPanel::UpdateLayout(int cx, int cy)
{
    // Controls row: y = header + padding
    int nCtrlY = MAP_HEADER_H + MAP_PADDING;
    int nX     = MAP_PADDING;

    // ComboBox: height 200 allows dropdown to show
    m_cmbSource.SetWindowPos(nullptr, nX, nCtrlY,
        MAP_COMBO_W, 200, SWP_NOZORDER | SWP_NOACTIVATE);
    nX += MAP_COMBO_W + MAP_PADDING;

    m_edtTarget.SetWindowPos(nullptr, nX, nCtrlY,
        MAP_EDIT_W, MAP_CONTROLS_H, SWP_NOZORDER | SWP_NOACTIVATE);
    nX += MAP_EDIT_W + MAP_PADDING;

    // Checkbox: vertically centered within the controls row
    int nChkY = nCtrlY + (MAP_CONTROLS_H - 16) / 2;
    m_chkRequired.SetWindowPos(nullptr, nX, nChkY,
        MAP_CHK_W, 16, SWP_NOZORDER | SWP_NOACTIVATE);
    nX += MAP_CHK_W + MAP_BTN_GAP;

    m_btnAdd.SetWindowPos(nullptr, nX, nCtrlY,
        MAP_BTN_ADD_W, MAP_CONTROLS_H, SWP_NOZORDER | SWP_NOACTIVATE);

    // Buttons row
    int nBtnY = nCtrlY + MAP_CONTROLS_H + MAP_ROW_GAP;
    nX = MAP_PADDING;

    m_btnDelete.SetWindowPos(nullptr, nX, nBtnY,
        MAP_BTN_DELETE_W, MAP_BUTTONS_H, SWP_NOZORDER | SWP_NOACTIVATE);
    nX += MAP_BTN_DELETE_W + MAP_BTN_GAP;

    m_btnAutoMap.SetWindowPos(nullptr, nX, nBtnY,
        MAP_BTN_AUTOMAP_W, MAP_BUTTONS_H, SWP_NOZORDER | SWP_NOACTIVATE);
    nX += MAP_BTN_AUTOMAP_W + MAP_BTN_GAP;

    m_btnClear.SetWindowPos(nullptr, nX, nBtnY,
        MAP_BTN_CLEAR_W, MAP_BUTTONS_H, SWP_NOZORDER | SWP_NOACTIVATE);

    // List: fills remaining space below buttons row
    int nListY = nBtnY + MAP_BUTTONS_H + MAP_PADDING;
    int nListH = cy - nListY;
    if (nListH < 0)
        nListH = 0;

    m_lstRules.SetWindowPos(nullptr, 0, nListY,
        cx, nListH, SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL MappingPanel::OnEraseBkgnd(CDC* pDC)
{
    CRect rect;
    GetClientRect(&rect);
    pDC->FillSolidRect(&rect, COLOR_WHITE);
    return TRUE;
}

void MappingPanel::OnPaint()
{
    CPaintDC dc(this);

    CRect rcClient;
    GetClientRect(&rcClient);
    dc.FillSolidRect(&rcClient, COLOR_WHITE);

    // Header background
    CRect rcHeader(0, 0, rcClient.right, MAP_HEADER_H);
    dc.FillSolidRect(&rcHeader, COLOR_SURFACE);

    // Header bottom border
    CRect rcBorder(0, MAP_HEADER_H - 1, rcClient.right, MAP_HEADER_H);
    dc.FillSolidRect(&rcBorder, COLOR_BORDER);

    // Title text
    CString strTitle;
    strTitle.LoadString(IDS_VIEW_MAPPING_TITLE);
    CFont* pOldFont = (CFont*)dc.SelectStockObject(DEFAULT_GUI_FONT);
    dc.SetTextColor(COLOR_TEXT);
    dc.SetBkMode(TRANSPARENT);
    CRect rcText = rcHeader;
    rcText.left += MAP_PADDING;
    dc.DrawText(strTitle, &rcText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    dc.SelectObject(pOldFont);
}

void MappingPanel::SetSourceColumns(const std::vector<CString>& arrColumns)
{
    m_arrSourceColumns = arrColumns;

    if (m_cmbSource.GetSafeHwnd() == nullptr)
        return;

    m_cmbSource.ResetContent();
    for (const CString& str : m_arrSourceColumns) {
        m_cmbSource.AddString(str);
    }

    UpdateButtonStates();
}

void MappingPanel::ClearAll()
{
    m_arrRules.clear();
    m_arrSourceColumns.clear();

    if (m_lstRules.GetSafeHwnd() != nullptr)
        m_lstRules.DeleteAllItems();

    if (m_cmbSource.GetSafeHwnd() != nullptr)
        m_cmbSource.ResetContent();

    if (m_edtTarget.GetSafeHwnd() != nullptr)
        m_edtTarget.SetWindowText(_T(""));

    if (m_chkRequired.GetSafeHwnd() != nullptr)
        m_chkRequired.SetCheck(BST_UNCHECKED);

    UpdateButtonStates();
}

void MappingPanel::RebuildList()
{
    m_lstRules.DeleteAllItems();

    CString strMark;
    strMark.LoadString(IDS_VIEW_MAPPING_REQUIRED_MARK);

    for (int i = 0; i < (int)m_arrRules.size(); i++) {
        const MappingRule& rule = m_arrRules[i];

        CString strNum;
        strNum.Format(_T("%d"), i + 1);

        LVITEM lvi  = {};
        lvi.mask    = LVIF_TEXT;
        lvi.iItem   = i;
        lvi.pszText = (LPTSTR)(LPCTSTR)strNum;
        int nIdx = m_lstRules.InsertItem(&lvi);

        m_lstRules.SetItemText(nIdx, 1, rule.GetSourceColumn());
        m_lstRules.SetItemText(nIdx, 2, rule.GetTargetColumn());
        m_lstRules.SetItemText(nIdx, 3, rule.IsRequired() ? (LPCTSTR)strMark : _T(""));
    }

    UpdateButtonStates();
}

void MappingPanel::UpdateButtonStates()
{
    BOOL bHasSource = (m_cmbSource.GetSafeHwnd() != nullptr &&
                       m_cmbSource.GetCount() > 0);
    BOOL bHasRules  = !m_arrRules.empty();
    BOOL bSelected  = (m_lstRules.GetSafeHwnd() != nullptr &&
                       m_lstRules.GetSelectedCount() > 0);

    if (m_btnAdd.GetSafeHwnd() != nullptr)
        m_btnAdd.EnableWindow(bHasSource);

    if (m_btnDelete.GetSafeHwnd() != nullptr)
        m_btnDelete.EnableWindow(bSelected);

    if (m_btnAutoMap.GetSafeHwnd() != nullptr)
        m_btnAutoMap.EnableWindow(bHasSource);

    if (m_btnClear.GetSafeHwnd() != nullptr)
        m_btnClear.EnableWindow(bHasRules);
}

void MappingPanel::AddRule()
{
    int nSel = m_cmbSource.GetCurSel();
    if (nSel == CB_ERR)
        return;

    CString strSource;
    m_cmbSource.GetLBText(nSel, strSource);

    CString strTarget;
    m_edtTarget.GetWindowText(strTarget);
    strTarget.Trim();

    // Target이 비어 있으면 Source 이름을 그대로 사용
    if (strTarget.IsEmpty())
        strTarget = strSource;

    BOOL bRequired = (m_chkRequired.GetCheck() == BST_CHECKED);
    m_arrRules.emplace_back(strSource, strTarget, bRequired);

    RebuildList();

    // 입력 컨트롤 초기화
    m_cmbSource.SetCurSel(-1);
    m_edtTarget.SetWindowText(_T(""));
    m_chkRequired.SetCheck(BST_UNCHECKED);
}

void MappingPanel::DeleteSelectedRule()
{
    int nSel = m_lstRules.GetNextItem(-1, LVNI_SELECTED);
    if (nSel < 0 || nSel >= (int)m_arrRules.size())
        return;

    m_arrRules.erase(m_arrRules.begin() + nSel);
    RebuildList();
}

void MappingPanel::AutoMap()
{
    m_arrRules.clear();
    for (const CString& strCol : m_arrSourceColumns) {
        m_arrRules.emplace_back(strCol, strCol, FALSE);
    }
    RebuildList();
}

void MappingPanel::OnBnClickedAdd()
{
    AddRule();
}

void MappingPanel::OnBnClickedDelete()
{
    DeleteSelectedRule();
}

void MappingPanel::OnBnClickedAutoMap()
{
    AutoMap();
}

void MappingPanel::OnBnClickedClear()
{
    m_arrRules.clear();
    RebuildList();
}

void MappingPanel::OnLvnItemChangedRules(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
    *pResult = 0;
    UpdateButtonStates();
}
