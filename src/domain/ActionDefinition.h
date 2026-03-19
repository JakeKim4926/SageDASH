
#pragma once

#include "EnumDefine.h"

// ============================================================
// ActionDefinition — 후속 액션 설정 데이터 (Phase 5)
// AutomationProject에 포함되어 .sagep 파일로 저장/불러오기 된다.
// ActionType에 따라 유효한 필드가 달라진다.
//   ACTION_EMAIL : 추가 파라미터 없음 (기본 메일 클라이언트 사용)
//   ACTION_API   : m_strUrl, m_strMethod
//   ACTION_FTP   : m_strHost, m_nPort, m_strUser, m_strPass,
//                  m_strRemoteDir, m_strFilename
// ============================================================
class ActionDefinition
{
public:
    ActionDefinition()
        : m_eType(ACTION_EMAIL)
        , m_nPort(21)
    {}

    explicit ActionDefinition(ActionType eType)
        : m_eType(eType)
        , m_nPort(21)
    {}

    ActionType GetType()       const { return m_eType; }
    void       SetType(ActionType e) { m_eType = e; }

    // API
    CString GetUrl()              const { return m_strUrl; }
    CString GetMethod()           const { return m_strMethod; }
    void    SetUrl(const CString& v)    { m_strUrl = v; }
    void    SetMethod(const CString& v) { m_strMethod = v; }

    // FTP
    CString GetHost()             const { return m_strHost; }
    int     GetPort()             const { return m_nPort; }
    CString GetUser()             const { return m_strUser; }
    CString GetPass()             const { return m_strPass; }
    CString GetRemoteDir()        const { return m_strRemoteDir; }
    CString GetFilename()         const { return m_strFilename; }
    void    SetHost(const CString& v)      { m_strHost = v; }
    void    SetPort(int n)                 { m_nPort = n; }
    void    SetUser(const CString& v)      { m_strUser = v; }
    void    SetPass(const CString& v)      { m_strPass = v; }
    void    SetRemoteDir(const CString& v) { m_strRemoteDir = v; }
    void    SetFilename(const CString& v)  { m_strFilename = v; }

private:
    ActionType m_eType;

    // ACTION_API
    CString m_strUrl;
    CString m_strMethod;

    // ACTION_FTP
    CString m_strHost;
    int     m_nPort;
    CString m_strUser;
    CString m_strPass;
    CString m_strRemoteDir;
    CString m_strFilename;
};
