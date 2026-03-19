
#pragma once

#include "TabularData.h"

// ============================================================
// IActionHandler — 자동화 후속 액션 인터페이스 (Phase 5-B)
// 데이터 처리 이후 실행되는 액션의 공통 계약
// ============================================================
class IActionHandler
{
public:
    virtual ~IActionHandler() = default;
    virtual CString GetName() const = 0;
    virtual void Execute(const TabularData& data) = 0;
};
