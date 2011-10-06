/********************************************************************
    created:    2010/02/11 22:06
    file:       DzStructsWin.h
    author:     Foreverflying
    purpose:    
*********************************************************************/

#ifndef __DzStructsWin_h__
#define __DzStructsWin_h__

#include "../DzIncOs.h"
#include "../DzDeclareStructs.h"
#include "../DzStructsList.h"

struct _DzOsStruct
{
    HANDLE                      iocp;
    void*                       originExceptPtr;
    char*                       originalStack;
    LPFN_ACCEPTEX               _AcceptEx;
    LPFN_CONNECTEX              _ConnectEx;
    LPFN_GETACCEPTEXSOCKADDRS   _GetAcceptExSockAddrs;
};

struct _DzThread
{
    DzLItr              lItr;
    void*               sp;
    char*               stack;
    char*               stackLimit;
    int                 sSize;
    int                 lastErr;
    int                 priority;
};

#endif // __DzStructsWin_h__
