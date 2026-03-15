
#include "pch.h"
#include "framework.h"
#include "ExcelInputReader.h"
#include "SageException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT ExcelInputReader::AutoWrap(int autoType, VARIANT* pvResult,
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

CString ExcelInputReader::VariantToCString(const VARIANT& v)
{
    CString str;
    switch (v.vt) {
    case VT_BSTR:
        str = CString(v.bstrVal);
        break;
    case VT_I4:
        str.Format(_T("%ld"), v.lVal);
        break;
    case VT_R8:
        str.Format(_T("%g"), v.dblVal);
        break;
    case VT_BOOL:
        str = v.boolVal ? _T("TRUE") : _T("FALSE");
        break;
    case VT_EMPTY:
    case VT_NULL:
        break;
    default: {
        VARIANT vStr;
        VariantInit(&vStr);
        if (SUCCEEDED(VariantChangeType(&vStr, const_cast<VARIANT*>(&v), 0, VT_BSTR)))
            str = CString(vStr.bstrVal);
        VariantClear(&vStr);
        break;
    }
    }
    return str;
}

void ExcelInputReader::Read(const CString& strFilePath, TabularData& outData)
{
    CLSID clsid;
    HRESULT hr = CLSIDFromProgID(L"Excel.Application", &clsid);
    if (FAILED(hr))
        throw SageException(_T("Excel이 설치되어 있지 않습니다."), strFilePath);

    IDispatch* pXlApp = nullptr;
    hr = CoCreateInstance(clsid, nullptr, CLSCTX_LOCAL_SERVER,
                          IID_IDispatch, (void**)&pXlApp);
    if (FAILED(hr) || pXlApp == nullptr)
        throw SageException(_T("Excel 애플리케이션을 시작할 수 없습니다."), strFilePath);

    VARIANT vFalse;
    vFalse.vt = VT_BOOL; vFalse.boolVal = VARIANT_FALSE;
    AutoWrap(DISPATCH_PROPERTYPUT, nullptr, pXlApp, L"Visible", 1, vFalse);
    AutoWrap(DISPATCH_PROPERTYPUT, nullptr, pXlApp, L"DisplayAlerts", 1, vFalse);

    VARIANT vWbs;
    VariantInit(&vWbs);
    AutoWrap(DISPATCH_PROPERTYGET, &vWbs, pXlApp, L"Workbooks", 0);
    if (vWbs.vt != VT_DISPATCH || vWbs.pdispVal == nullptr) {
        AutoWrap(DISPATCH_METHOD, nullptr, pXlApp, L"Quit", 0);
        pXlApp->Release();
        throw SageException(_T("Excel Workbooks 컬렉션을 가져올 수 없습니다."), strFilePath);
    }
    IDispatch* pWorkbooks = vWbs.pdispVal;

    VARIANT vPath;
    vPath.vt      = VT_BSTR;
    vPath.bstrVal = strFilePath.AllocSysString();
    VARIANT vWb;
    VariantInit(&vWb);
    AutoWrap(DISPATCH_METHOD, &vWb, pWorkbooks, L"Open", 1, vPath);
    SysFreeString(vPath.bstrVal);
    pWorkbooks->Release();

    if (vWb.vt != VT_DISPATCH || vWb.pdispVal == nullptr) {
        AutoWrap(DISPATCH_METHOD, nullptr, pXlApp, L"Quit", 0);
        pXlApp->Release();
        throw SageException(_T("Excel 파일을 열 수 없습니다."), strFilePath);
    }
    IDispatch* pWorkbook = vWb.pdispVal;

    outData.m_strFilePath = strFilePath;

    VARIANT vSheets;
    VariantInit(&vSheets);
    AutoWrap(DISPATCH_PROPERTYGET, &vSheets, pWorkbook, L"Worksheets", 0);
    IDispatch* pSheets = vSheets.pdispVal;

    VARIANT vCount;
    VariantInit(&vCount);
    AutoWrap(DISPATCH_PROPERTYGET, &vCount, pSheets, L"Count", 0);
    int nSheetCount = (vCount.vt == VT_I4) ? vCount.lVal : 1;

    for (int iSheet = 1; iSheet <= nSheetCount; iSheet++) {
        VARIANT vIdx;
        vIdx.vt = VT_I4; vIdx.lVal = iSheet;
        VARIANT vSheet;
        VariantInit(&vSheet);
        AutoWrap(DISPATCH_PROPERTYGET, &vSheet, pSheets, L"Item", 1, vIdx);
        if (vSheet.vt != VT_DISPATCH || vSheet.pdispVal == nullptr)
            continue;
        IDispatch* pSheet = vSheet.pdispVal;

        VARIANT vName;
        VariantInit(&vName);
        AutoWrap(DISPATCH_PROPERTYGET, &vName, pSheet, L"Name", 0);

        outData.m_arrSheets.emplace_back();
        DataSheet& sheet = outData.m_arrSheets.back();
        sheet.m_strName = (vName.vt == VT_BSTR) ? CString(vName.bstrVal) : _T("Sheet");
        VariantClear(&vName);

        VARIANT vRange;
        VariantInit(&vRange);
        AutoWrap(DISPATCH_PROPERTYGET, &vRange, pSheet, L"UsedRange", 0);
        if (vRange.vt != VT_DISPATCH || vRange.pdispVal == nullptr) {
            pSheet->Release();
            continue;
        }
        IDispatch* pRange = vRange.pdispVal;

        VARIANT vValues;
        VariantInit(&vValues);
        AutoWrap(DISPATCH_PROPERTYGET, &vValues, pRange, L"Value2", 0);
        pRange->Release();

        if ((vValues.vt & VT_ARRAY) && vValues.parray != nullptr) {
            SAFEARRAY* psa = vValues.parray;
            LONG lRowLo, lRowHi, lColLo, lColHi;
            SafeArrayGetLBound(psa, 1, &lRowLo);
            SafeArrayGetUBound(psa, 1, &lRowHi);
            SafeArrayGetLBound(psa, 2, &lColLo);
            SafeArrayGetUBound(psa, 2, &lColHi);

            for (LONG r = lRowLo; r <= lRowHi; r++) {
                std::vector<CString> row;
                for (LONG c = lColLo; c <= lColHi; c++) {
                    LONG idx[2] = { r, c };
                    VARIANT vCell;
                    VariantInit(&vCell);
                    SafeArrayGetElement(psa, idx, &vCell);
                    row.push_back(VariantToCString(vCell));
                    VariantClear(&vCell);
                }
                sheet.m_arrRows.push_back(std::move(row));
            }
        } else if (vValues.vt != VT_EMPTY && vValues.vt != VT_NULL) {
            std::vector<CString> row;
            row.push_back(VariantToCString(vValues));
            sheet.m_arrRows.push_back(std::move(row));
        }
        VariantClear(&vValues);

        pSheet->Release();
    }

    pSheets->Release();

    VARIANT vSave;
    vSave.vt = VT_BOOL; vSave.boolVal = VARIANT_FALSE;
    AutoWrap(DISPATCH_METHOD, nullptr, pWorkbook, L"Close", 1, vSave);
    pWorkbook->Release();

    AutoWrap(DISPATCH_METHOD, nullptr, pXlApp, L"Quit", 0);
    pXlApp->Release();

    if (outData.m_arrSheets.empty() || outData.m_arrSheets[0].GetRowCount() == 0)
        throw SageException(_T("Excel 파일이 비어 있습니다."), strFilePath);
}
