
#pragma once

#include "IExecutionStep.h"

// ============================================================
// MappingStep — 파이프라인 2단계: 컬럼 매핑 적용
// 매핑 규칙에 따라 Source 컬럼을 Target 이름으로 변환하고
// ctx.m_transformedSheet 에 결과를 저장한다.
// 매핑 규칙이 없으면 원본을 그대로 통과시킨다.
// ============================================================
class MappingStep : public IExecutionStep
{
public:
    CString GetName() const override;
    BOOL    Execute(ExecutionContext& ctx) override;

private:
    // header 에서 strColumn 의 인덱스를 반환. 없으면 -1
    int FindColumnIndex(const std::vector<CString>& header, const CString& strColumn) const;
};
