
#pragma once

#include "IOutputTarget.h"

// ============================================================
// FileOutputTarget — 파일 경로 기반 IOutputTarget 구현
// ============================================================
class FileOutputTarget : public IOutputTarget
{
public:
    explicit FileOutputTarget(const CString& strFilePath)
        : m_strFilePath(strFilePath)
    {}

    CString GetDescription() const override { return m_strFilePath; }
    const CString& GetFilePath() const { return m_strFilePath; }

private:
    CString m_strFilePath;
};
