
#include "pch.h"
#include "framework.h"
#include "SageMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

SageMgr& SageMgr::GetInstance()
{
    static SageMgr instance;
    return instance;
}

void SageMgr::Initialize()
{
    // 추후 확장: 설정 로드, 서비스 초기화 등
}

void SageMgr::Shutdown()
{
    // 추후 확장: 리소스 정리
}

void SageMgr::Log(const CString& strMessage)
{
    m_logger.Write(strMessage);
}

SageLogger& SageMgr::GetLogger()
{
    return m_logger;
}
