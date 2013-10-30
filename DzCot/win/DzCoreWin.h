/********************************************************************
    created:    2010/02/11 21:50
    file:       DzCoreWin.h
    author:     Foreverflying
    purpose:    
********************************************************************/

#ifndef __DzCoreWin_h__
#define __DzCoreWin_h__

#include "../DzBase.h"
#include "../DzResourceMgr.h"
#include "../DzSynObj.h"

#ifdef __cplusplus
extern "C"{
#endif

void __stdcall CallDzCotEntry( void );
void __stdcall DzCotEntry(
    DzHost*             host,
    DzEntry volatile*   entryPtr,
    intptr_t volatile*  contextPtr
    );
BOOL InitOsStruct( DzHost* host );
void CleanOsStruct( DzHost* host );

inline void InitDzCot( DzHost* host, DzCot* dzCot )
{
    __Dbg( InitDzCot )( host, dzCot );
}

inline void InitDzFd( DzFd* dzFd )
{
    dzFd->ref = 0;
}

inline DzFd* CreateDzFd( DzHost* host )
{
    DzFd* dzFd;

    if( !host->dzFdPool ){
        if( !AllocDzFdPool( host ) ){
            return NULL;
        }
    }
    dzFd = MEMBER_BASE( host->dzFdPool, DzFd, lItr );
    host->dzFdPool = host->dzFdPool->next;
    dzFd->err = 0;
    dzFd->ref++;
    return dzFd;
}

inline void CloneDzFd( DzFd* dzFd )
{
    dzFd->ref++;
}

inline void CloseDzFd( DzHost* host, DzFd* dzFd )
{
    dzFd->ref--;
    if( dzFd->ref == 0 ){
        dzFd->lItr.next = host->dzFdPool;
        host->dzFdPool = &dzFd->lItr;
    }
}

#if defined( _X86_ )

struct DzStackBottom
{
    void*       stackLimit;
    void*       stackPtr;
    void*       exceptPtr;
    void*       _unusedEdi;
    void*       _unusedEsi;
    void*       _unusedEbx;
    void*       _unusedEbp;
    void*       ipEntry;
    DzHost*     host;
    DzEntry     entry;
    intptr_t    context;
};

#elif defined( _M_AMD64 )

struct DzStackBottom
{
    void*       stackLimit;
    void*       stackPtr;
    void*       _unusedR15;
    void*       _unusedR14;
    void*       _unusedR13;
    void*       _unusedR12;
    void*       _unusedRdi;
    void*       _unusedRsi;
    void*       _unusedRbx;
    void*       _unusedRbp;
    void*       ipEntry;
    DzHost*     host;
    DzEntry     entry;
    intptr_t    context;
    void*       _unusedPadding;
};

#endif

inline void SetCotEntry( DzCot* dzCot, DzEntry entry, intptr_t context )
{
    ( ( (struct DzStackBottom*)dzCot->stack ) - 1 )->entry = entry;
    ( ( (struct DzStackBottom*)dzCot->stack ) - 1 )->context = context;
}

inline char* AllocStack( DzHost* host, int size )
{
    char* base;
    
    base = (char*)VirtualAlloc(
        NULL,
        size,
        MEM_RESERVE | MEM_TOP_DOWN,
        PAGE_READWRITE
        );

    return base ? base + size : NULL;
}

inline void FreeStack( char* stack, int size )
{
    VirtualFree( stack - size, 0, MEM_RELEASE );
}

inline char* CommitStack( char* stack, int size )
{
    void* tmp;
    BOOL ret;

    tmp = VirtualAlloc(
        stack - size,
        size,
        MEM_COMMIT,
        PAGE_READWRITE
        );
    if( !tmp ){
        return NULL;
    }
    ret = VirtualProtect(
        stack - size,
        PAGE_SIZE,
        PAGE_READWRITE | PAGE_GUARD,
        (LPDWORD)&tmp
        );
    return ret ? stack - size + PAGE_SIZE : NULL;
}

inline void InitCotStack( DzHost* host, DzCot* dzCot )
{
    struct DzStackBottom* bottom;

    bottom = ( (struct DzStackBottom*)dzCot->stack ) - 1;
    bottom->host = host;
    bottom->ipEntry = CallDzCotEntry;
#ifdef _X86_
    bottom->exceptPtr = host->os.originExceptPtr;
#endif
    bottom->stackPtr = dzCot->stack;
    bottom->stackLimit = dzCot->stackLimit;

    dzCot->sp = bottom;
}

inline DzCot* InitCot( DzHost* host, DzCot* dzCot, int sType )
{
    int size;
    
    size = host->cotStackSize[ sType ];
    if( size < DZ_MIN_PAGE_STACK_SIZE ){
        dzCot->stackLimit = (char*)AllocChunk( host, size );
        if( !dzCot->stackLimit ){
            return NULL;
        }
        dzCot->stack = dzCot->stackLimit + size;
    }else{
        dzCot->stack = AllocStack( host, size );
        if( !dzCot->stack ){
            return NULL;
        }
        dzCot->stackLimit = CommitStack( dzCot->stack, PAGE_SIZE * 3 );
        if( !dzCot->stackLimit )
        {
            return NULL;
        }
    }
    dzCot->sType = sType;
    InitCotStack( host, dzCot );
    return dzCot;
}

inline void FreeCotStack( DzHost* host, DzCot* dzCot )
{
    FreeStack( dzCot->stack, host->cotStackSize[ dzCot->sType ] );
}

#ifdef __cplusplus
};
#endif

#endif // __DzCoreWin_h__
