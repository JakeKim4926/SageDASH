
#pragma once

#include "IInputReader.h"

// CSV 파일을 읽어 CTabularData로 변환한다
// 실패 시 SageException을 던진다
class CsvInputReader : public IInputReader
{
public:
    void Read(const CString& strFilePath, CTabularData& outData) override;

private:
    std::vector<CString> ParseLine(const CString& strLine);
};
