
#include "pch.h"
#include "framework.h"
#include "ExcelOutputWriter.h"
#include "FileOutputTarget.h"
#include "SageException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static const int XL_OPEN_XML_WORKBOOK = 51; // xlOpenXMLWorkbook

HRESULT ExcelOutputWriter::AutoWrap(int autoType, VARIANT* pvResult,
                                    IDispatch* pDisp, LPOLESTR ptName, int cArgs, ...)
{
    va_list marker;
    va_start(marker, cArgs);

    DISPPARAMS dp      = { nullptr, nullptr, 0, 0 };
    DISPID dispidNamed = DISPID_PROPERTYPUT;
    DISPID dispID      = 0;

    HRESULT hr = pDisp->GetIDsOfNames(IID_NULL, &ptName, 1,
                                       LOCALE_USER_DEFAULT, &dispID);
    if (FAILED(hr)) {
        va_end(marker);
        return hr;
    }

    std::vector<VARIANT> args(cArgs);
    for (int i = 0; i < cArgs; i++)
        args[cArgs - 1 - i] = va_arg(marker, VARIANT);
    va_end(marker);

    dp.cArgs  = cArgs;
    dp.rgvarg = cArgs > 0 ? args.data() : nullptr;

    if (autoType & DISPATCH_PROPERTYPUT) {
        dp.cNamedArgs        = 1;
        dp.rgdispidNamedArgs = &dispidNamed;
    }

    hr = pDisp->Invoke(dispID, IID_NULL, LOCALE_SYSTEM_DEFAULT,
                       autoType, &dp, pvResult, nullptr, nullptr);
    return hr;
}

void ExcelOutputWriter::Write(const TabularData& data, const IOutputTarget& target)
{
    const FileOutputTarget* pFileTarget =
        dynamic_cast<const FileOutputTarget*>(&target);
    if (pFileTarget == nullptr)
        throw SageException(_T("ExcelOutputWriter: 잘못된 출력 대상입니다."));

    if (data.GetSheetCount() == 0 || data.GetSheet(0).GetRowCount() == 0)
        throw SageException(_T("내보낼 데이터가 없습니다."));

    const CString& strPath = pFileTarget->GetFilePath();

    CLSID clsid;
    HRESULT hr = CLSIDFromProgID(L"Excel.Application", &clsid);
    if (FAILED(hr))
        throw SageException(_T("Excel이 설치되어 있지 않습니다."), strPath);

    IDispatch* pXlApp = nullptr;
    hr = CoCreateInstance(clsid, nullptr, CLSCTX_LOCAL_SERVER,
                          IID_IDispatch, (void**)&pXlApp);
    if (FAILED(hr) || pXlApp == nullptr)
        throw SageException(_T("Excel 애플리케이션을 시작할 수 없습니다."), strPath);

    VARIANT vFalse;
    vFalse.vt = VT_BOOL; vFalse.boolVal = VARIANT_FALSE;
    AutoWrap(DISPATCH_PROPERTYPUT, nullptr, pXlApp, L"Visible", 1, vFalse);
    AutoWrap(DISPATCH_PROPERTYPUT, nullptr, pXlApp, L"DisplayAlerts", 1, vFalse);

    // Workbooks.Add()
    VARIANT vWbs;
    VariantInit(&vWbs);
    AutoWrap(DISPATCH_PROPERTYGET, &vWbs, pXlApp, L"Workbooks", 0);
    if (vWbs.vt != VT_DISPATCH || vWbs.pdispVal == nullptr) {
        AutoWrap(DISPATCH_METHOD, nullptr, pXlApp, L"Quit", 0);
        pXlApp->Release();
        throw SageException(_T("Excel Workbooks 컬렉션을 가져올 수 없습니다."), strPath);
    }
    IDispatch* pWorkbooks = vWbs.pdispVal;

    VARIANT vWb;
    VariantInit(&vWb);
    AutoWrap(DISPATCH_METHOD, &vWb, pWorkbooks, L"Add", 0);
    pWorkbooks->Release();

    if (vWb.vt != VT_DISPATCH || vWb.pdispVal == nullptr) {
        AutoWrap(DISPATCH_METHOD, nullptr, pXlApp, L"Quit", 0);
        pXlApp->Release();
        throw SageException(_T("새 워크북을 만들 수 없습니다."), strPath);
    }
    IDispatch* pWorkbook = vWb.pdispVal;

    // Worksheets(1)
    VARIANT vSheets;
    VariantInit(&vSheets);
    AutoWrap(DISPATCH_PROPERTYGET, &vSheets, pWorkbook, L"Worksheets", 0);
    if (vSheets.vt != VT_DISPATCH || vSheets.pdispVal == nullptr) {
        AutoWrap(DISPATCH_METHOD, nullptr, pWorkbook, L"Close", 0);
        pWorkbook->Release();
        AutoWrap(DISPATCH_METHOD, nullptr, pXlApp, L"Quit", 0);
        pXlApp->Release();
        throw SageException(_T("워크시트를 가져올 수 없습니다."), strPath);
    }
    IDispatch* pSheets = vSheets.pdispVal;

    VARIANT vIdx;
    vIdx.vt = VT_I4; vIdx.lVal = 1;
    VARIANT vSheet;
    VariantInit(&vSheet);
    AutoWrap(DISPATCH_PROPERTYGET, &vSheet, pSheets, L"Item", 1, vIdx);
    pSheets->Release();

    if (vSheet.vt != VT_DISPATCH || vSheet.pdispVal == nullptr) {
        AutoWrap(DISPATCH_METHOD, nullptr, pWorkbook, L"Close", 0);
        pWorkbook->Release();
        AutoWrap(DISPATCH_METHOD, nullptr, pXlApp, L"Quit", 0);
        pXlApp->Release();
        throw SageException(_T("첫 번째 시트를 가져올 수 없습니다."), strPath);
    }
    IDispatch* pSheet = vSheet.pdispVal;

    // 데이터 쓰기: sheet.Cells(r, c).Value = cell
    const DataSheet& sheet0 = data.GetSheet(0);
    int nRows = sheet0.GetRowCount();
    for (int r = 0; r < nRows; r++) {
        const std::vector<CString>& row = sheet0.m_arrRows[r];
        for (int c = 0; c < (int)row.size(); c++) {
            // Cells(r+1, c+1)
            VARIANT vRow, vCol;
            vRow.vt = VT_I4; vRow.lVal = r + 1;
            vCol.vt = VT_I4; vCol.lVal = c + 1;

            VARIANT vCell;
            VariantInit(&vCell);
            AutoWrap(DISPATCH_PROPERTYGET, &vCell, pSheet, L"Cells", 2, vRow, vCol);
            if (vCell.vt != VT_DISPATCH || vCell.pdispVal == nullptr)
                continue;
            IDispatch* pCell = vCell.pdispVal;

            VARIANT vVal;
            vVal.vt       = VT_BSTR;
            vVal.bstrVal  = row[c].AllocSysString();
            AutoWrap(DISPATCH_PROPERTYPUT, nullptr, pCell, L"Value", 1, vVal);
            SysFreeString(vVal.bstrVal);
            pCell->Release();
        }
    }
    pSheet->Release();

    // SaveAs(filePath, XlFileFormat=51)
    VARIANT vSavePath, vFormat;
    vSavePath.vt      = VT_BSTR;
    vSavePath.bstrVal = strPath.AllocSysString();
    vFormat.vt  = VT_I4;
    vFormat.lVal = XL_OPEN_XML_WORKBOOK;
    AutoWrap(DISPATCH_METHOD, nullptr, pWorkbook, L"SaveAs", 2, vSavePath, vFormat);
    SysFreeString(vSavePath.bstrVal);

    VARIANT vSaveNo;
    vSaveNo.vt = VT_BOOL; vSaveNo.boolVal = VARIANT_FALSE;
    AutoWrap(DISPATCH_METHOD, nullptr, pWorkbook, L"Close", 1, vSaveNo);
    pWorkbook->Release();

    AutoWrap(DISPATCH_METHOD, nullptr, pXlApp, L"Quit", 0);
    pXlApp->Release();
}
