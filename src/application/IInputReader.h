
#pragma once

#include "TabularData.h"

class IInputReader
{
public:
    virtual ~IInputReader() = default;
    virtual void Read(const CString& strFilePath, TabularData& outData) = 0;
};
