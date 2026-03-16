
#pragma once

#include "ValidationRule.h"
#include "ValidationResult.h"
#include "TabularData.h"
#include <vector>

// ============================================================
// ValidationService — DataSheet를 규칙 목록으로 검증 (Phase 3)
// ============================================================
class ValidationService
{
public:
    ValidationResult Validate(const DataSheet& sheet,
                              const std::vector<ValidationRule>& arrRules) const;
};
