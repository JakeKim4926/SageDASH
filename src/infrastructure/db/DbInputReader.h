
#pragma once

#include "IInputReader.h"

class DbInputReader : public IInputReader
{
public:
    void Read(const CString& strDbqPath, TabularData& outData) override;

private:
    static BOOL ParseDbqFile(const CString& strPath,
                             CString& strConnStr,
                             CString& strQuery);
    static void ExecuteQuery(const CString& strConnStr,
                             const CString& strQuery,
                             TabularData& outData);
};
