
#pragma once

#include "EnumDefine.h"

// ============================================================
// ActionDefinition — 후속 액션 정의 (도메인 값 객체)
// ============================================================
// Param 매핑:
//   EMAIL : 없음 (Simple MAPI — 발송 시 UI에서 입력)
//   API   : m_strParam1=URL, m_strParam2=Method
//   FTP   : m_strParam1=Host, m_strParam2=Port,
//            m_strParam3=Username, m_strParam4=Password,
//            m_strParam5=RemoteDir, m_strParam6=Filename
// ============================================================
class ActionDefinition
{
public:
    ActionDefinition() : m_type(ACTION_TYPE_NONE) {}

    ActionType m_type;

    CString    m_strParam1;  // API: URL       / FTP: Host
    CString    m_strParam2;  // API: Method    / FTP: Port
    CString    m_strParam3;  //                  FTP: Username
    CString    m_strParam4;  //                  FTP: Password
    CString    m_strParam5;  //                  FTP: RemoteDir
    CString    m_strParam6;  //                  FTP: Filename
};
