
#pragma once

#include "IInputReader.h"

class CsvInputReader : public IInputReader
{
public:
    void Read(const CString& strFilePath, TabularData& outData) override;

private:
    TCHAR DetectDelimiter(const CString& strFirstLine);
    std::vector<CString> ParseLine(const CString& strLine, TCHAR chDelim);
};
