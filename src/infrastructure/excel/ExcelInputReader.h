
#pragma once

#include "IInputReader.h"

class ExcelInputReader : public IInputReader
{
public:
    void Read(const CString& strFilePath, TabularData& outData) override;

private:
    static HRESULT AutoWrap(int autoType, VARIANT* pvResult,
                            IDispatch* pDisp, LPOLESTR ptName, int cArgs, ...);
    static CString VariantToCString(const VARIANT& v);
};
