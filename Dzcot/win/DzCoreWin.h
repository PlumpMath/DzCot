/********************************************************************
    created:    2010/02/11 21:50
    file:       DzCoreWin.h
    author:     Foreverflying
    purpose:    
*********************************************************************/

#ifndef __DzCoreWin_h__
#define __DzCoreWin_h__

#include "../DzBaseOs.h"
#include "../DzResourceMgr.h"
#include "../DzSynObj.h"

#ifdef __cplusplus
extern "C"{
#endif

BOOL InitOsStruct( DzHost* host, DzHost* parentHost );
void DeleteOsStruct( DzHost* host, DzHost* parentHost );
void __stdcall DzcotRoutine( DzRoutine entry, void* context );

inline void InitDzThread( DzThread* dzThread )
{
}

#if defined( _X86_ )

struct DzStackBottom
{
    void*       stackLimit;
    void*       stackPtr;
    void*       exceptPtr;
    void*       unusedEdi;
    void*       unusedEsi;
    void*       unusedEbx;
    void*       unusedEbp;
    void*       routineEntry;
    void*       unusedEip;
    DzRoutine   entry;
    void*       context;
};

#define DzcotRoutineEntry DzcotRoutine

#elif defined( _M_AMD64 )

struct DzStackBottom
{
    void*       stackLimit;
    void*       stackPtr;
    void*       unusedR15;
    void*       unusedR14;
    void*       unusedR13;
    void*       unusedR12;
    void*       unusedRdi;
    void*       unusedRsi;
    void*       unusedRbx;
    void*       unusedRbp;
    void*       routineEntry;
    void*       unusedEip;
    DzRoutine   entry;
    void*       context;
    void*       unusedR8;
    void*       unusedR9;
};

void CallDzcotRoutine();

#define DzcotRoutineEntry CallDzcotRoutine

#endif

inline void SetThreadEntry( DzThread* dzThread, DzRoutine entry, void* context )
{
    struct DzStackBottom* bottom;

    bottom = ( (struct DzStackBottom*)dzThread->stack ) - 1;
    bottom->entry = entry;
    bottom->context = context;
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
    char* base;
    
    base = stack - size;
    VirtualFree( base, 0, MEM_RELEASE );
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

inline void InitCotStack( DzHost* host, DzThread* dzThread )
{
    struct DzStackBottom* bottom;

    bottom = ( (struct DzStackBottom*)dzThread->stack ) - 1;
    bottom->routineEntry = DzcotRoutineEntry;
#ifdef _X86_
    bottom->exceptPtr = host->osStruct.originExceptPtr;
#endif
    bottom->stackPtr = dzThread->stack;
    bottom->stackLimit = dzThread->stackLimit;

    dzThread->sp = bottom;
}

inline DzThread* InitCot( DzHost* host, DzThread* dzThread, int sSize )
{
    int size;
    
    size = DZ_STACK_UNIT_SIZE << ( sSize * DZ_STACK_SIZE_STEP );
    if( sSize <= DZ_MAX_PERSIST_STACK_SIZE ){
        dzThread->stackLimit = (char*)AllocChunk( host, size );
        if( !dzThread->stackLimit ){
            return NULL;
        }
        dzThread->stack = dzThread->stackLimit + size;
    }else{
        dzThread->stack = AllocStack( host, size );
        if( !dzThread->stack ){
            return NULL;
        }
        dzThread->stackLimit = CommitStack( dzThread->stack, PAGE_SIZE * 3 );
        if( !dzThread->stackLimit )
        {
            return NULL;
        }
    }
    dzThread->sSize = sSize;
    dzThread->lastErr = 0;
    InitCotStack( host, dzThread );
    return dzThread;
}

inline void FreeCotStack( DzThread* dzThread )
{
    FreeStack( dzThread->stack, DZ_STACK_UNIT_SIZE << ( dzThread->sSize * DZ_STACK_SIZE_STEP ) );
}

#ifdef __cplusplus
};
#endif

#endif // __DzCoreWin_h__
