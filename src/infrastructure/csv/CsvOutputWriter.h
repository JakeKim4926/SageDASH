
#pragma once

#include "IOutputWriter.h"

// ============================================================
// CsvOutputWriter — TabularData → CSV 파일 저장
// ============================================================
class CsvOutputWriter : public IOutputWriter
{
public:
    void Write(const TabularData& data, const IOutputTarget& target) override;

private:
    static CString EscapeCell(const CString& str);
};
