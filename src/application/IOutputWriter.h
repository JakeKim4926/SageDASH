
#pragma once

#include "TabularData.h"
#include "IOutputTarget.h"

// 출력 인터페이스
// Phase 3에서 구현 예정 — Phase 2에서는 인터페이스만 정의
// 실패 시 SageException을 던진다
class IOutputWriter
{
public:
    virtual ~IOutputWriter() = default;
    virtual void Write(const CTabularData& data, const IOutputTarget& target) = 0;
};
