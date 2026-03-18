
#pragma once

#include "IInputReader.h"

class FolderScanInputReader : public IInputReader
{
public:
    void Read(const CString& strFolderPath, TabularData& outData) override;

private:
    void ScanFolder(const CString& strFolderPath, DataSheet& outSheet);
    static CString FormatFileSize(ULONGLONG ullBytes);
    static CString FormatFileTime(const FILETIME& ft);
};
