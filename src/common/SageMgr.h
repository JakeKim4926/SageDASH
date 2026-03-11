
#pragma once

#include "SageLogger.h"

// SageMgr: 앱 공통 인프라 서비스 레지스트리 (싱글톤)
// sageMgr 매크로로 접근: sageMgr.Log(), sageMgr.GetLogger() 등
class SageMgr
{
public:
    static SageMgr& GetInstance();

    void Initialize();  // CSAGEDashApp::InitInstance()에서 호출
    void Shutdown();    // CSAGEDashApp::ExitInstance()에서 호출

    void        Log(const CString& strMessage);
    SageLogger& GetLogger();

private:
    SageMgr() {}
    SageMgr(const SageMgr&);

    SageLogger m_logger;
    BOOL    m_bInitialized = FALSE;
};

#define sageMgr SageMgr::GetInstance()
