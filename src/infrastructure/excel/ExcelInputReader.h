
#pragma once

#include "IInputReader.h"

// Excel (.xlsx/.xls) 파일을 읽어 CTabularData로 변환한다
// COM Automation (Excel.Application) 을 사용하므로 Excel 설치 필요
// 실패 시 SageException을 던진다
class ExcelInputReader : public IInputReader
{
public:
    void Read(const CString& strFilePath, CTabularData& outData) override;

private:
    static HRESULT AutoWrap(int autoType, VARIANT* pvResult,
                            IDispatch* pDisp, LPOLESTR ptName, int cArgs, ...);
    static CString VariantToCString(const VARIANT& v);
};
