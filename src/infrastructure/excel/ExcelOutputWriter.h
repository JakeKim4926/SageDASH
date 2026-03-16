
#pragma once

#include "IOutputWriter.h"

// ============================================================
// ExcelOutputWriter — TabularData → XLSX 파일 저장 (COM automation)
// Excel 설치 필요
// ============================================================
class ExcelOutputWriter : public IOutputWriter
{
public:
    void Write(const TabularData& data, const IOutputTarget& target) override;

private:
    static HRESULT AutoWrap(int autoType, VARIANT* pvResult,
                            IDispatch* pDisp, LPOLESTR ptName, int cArgs, ...);
};
