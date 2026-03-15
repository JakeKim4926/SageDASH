
#pragma once

// 출력 대상 추상화
// Phase 3에서 구현 예정 — Phase 2에서는 인터페이스만 정의
// 예: 파일 경로, 이메일 주소, API 엔드포인트 등
class IOutputTarget
{
public:
    virtual ~IOutputTarget() = default;
    virtual CString GetDescription() const = 0;
};
