
#pragma once

// 입력 소스 추상화
// Phase 2: 파일 경로만 지원, 이후 URL/DB 쿼리/폴더 스캔 등으로 확장
class IInputSource
{
public:
    virtual ~IInputSource() = default;
    virtual CString GetPath() const = 0;
};

class FileInputSource : public IInputSource
{
public:
    explicit FileInputSource(const CString& strPath) : m_strPath(strPath) {}
    CString GetPath() const override { return m_strPath; }

private:
    CString m_strPath;
};
