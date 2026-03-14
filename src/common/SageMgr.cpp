
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
    if (m_bInitialized)
        return;

    m_bInitialized = TRUE;
}

void SageMgr::Shutdown()
{
    if (!m_bInitialized)
        return;
}

void SageMgr::Log(const CString& strMessage)
{
    m_logger.Write(strMessage);
}

SageLogger& SageMgr::GetLogger()
{
    return m_logger;
}
