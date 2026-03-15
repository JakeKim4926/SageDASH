
#pragma once

#include "TabularData.h"

// 입력 소스에서 CTabularData를 읽어오는 인터페이스
// 실패 시 SageException을 던진다
class IInputReader
{
public:
    virtual ~IInputReader() = default;
    virtual void Read(const CString& strFilePath, CTabularData& outData) = 0;
};
