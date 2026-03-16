
#pragma once

#include "ExecutionContext.h"

// ============================================================
// IExecutionStep — 자동화 파이프라인 단계 인터페이스
// 각 단계(입력/매핑/검증/출력)는 이 인터페이스를 구현한다.
// ============================================================
class IExecutionStep
{
public:
    virtual ~IExecutionStep() = default;

    virtual CString GetName() const = 0;
    virtual BOOL    Execute(ExecutionContext& ctx) = 0;
};
