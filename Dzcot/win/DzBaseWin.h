/********************************************************************
    created:    2010/11/22 17:27
    file:       DzBaseWin.h
    author:     Foreverflying
    purpose:    
*********************************************************************/

#ifndef __DzBaseWin_h__
#define __DzBaseWin_h__

#include "../DzStructs.h"
#include "../../DzcotData/DzcotData.h"

#ifdef __cplusplus
extern "C"{
#endif

inline void* PageAlloc( size_t size )
{
    return VirtualAlloc( NULL, size, MEM_COMMIT, PAGE_READWRITE );
}

inline void* PageReserv( size_t size )
{
    return VirtualAlloc( NULL, size, MEM_RESERVE, PAGE_READWRITE );
}

inline void* PageCommit( void* p, size_t size )
{
    return VirtualAlloc( p, size, MEM_COMMIT, PAGE_READWRITE );
}

inline void PageFree( void* p, size_t size )
{
    VirtualFree( p, 0, MEM_RELEASE );
}

inline void InitTlsIndex()
{
#ifdef STORE_HOST_IN_ARBITRARY_USER_POINTER
#else
    if( tlsIndex == TLS_OUT_OF_INDEXES ){
        while( InterlockedExchange( &tlsLock, 1 ) == 1 );
        if( *(volatile int*)&tlsIndex == TLS_OUT_OF_INDEXES ){
            tlsIndex = TlsAlloc();
        }
        InterlockedExchange( &tlsLock, 0 );
    }
#endif
}

inline DzHost* GetHost()
{
#ifdef STORE_HOST_IN_ARBITRARY_USER_POINTER
#if defined( _X86_ )
    return (DzHost*)__readfsdword( 20 );
#elif defined( _M_AMD64 )
    return *(DzHost**)( __readgsqword( 0x30 ) + 40 );
#endif
#else
    return (DzHost*)TlsGetValue( tlsIndex );
#endif
}

inline void SetHost( DzHost* host )
{
#ifdef STORE_HOST_IN_ARBITRARY_USER_POINTER
#if defined( _X86_ )
    __writefsdword( 20, (DWORD)host );
#elif defined( _M_AMD64 )
    *(DzHost**)( __readgsqword( 0x30 ) + 40 ) = host;
#endif
#else
    TlsSetValue( tlsIndex, host );
#endif
}

inline void* GetExceptPtr()
{
#if defined( _X86_ )
    return (void*)__readfsdword( 0 );
#elif defined( _M_AMD64 )
    return NULL;
#endif
}

#ifdef SWITCH_COT_FLOAT_SAFE
void __fastcall DzSwitchFloatSafe( DzHost* host, DzThread* dzThread );
#define DzSwitch DzSwitchFloatSafe
#else
void __fastcall DzSwitchFast( DzHost* host, DzThread* dzThread );
#define DzSwitch DzSwitchFast
#endif


#ifdef __DBG_CHECK_COT_STACK_OVERFLOW

inline void __DbgCheckCotStackOverflow( int sSize )
{
    //TODO: find a better way to check

    int size;
    char* stack;
    char* stackLimit;
    char* tmp;

    size = DZ_STACK_UNIT_SIZE << ( sSize * DZ_STACK_SIZE_STEP );
#if defined( _X86_ )
    stack = (char*)__readfsdword( 4 );
    stackLimit = (char*)__readfsdword( 8 );
#elif defined( _M_AMD64 )
    stack = *(char**)( __readgsqword( 0x30 ) + 8 );
    stackLimit = *(char**)( __readgsqword( 0x30 ) + 16 );
#endif

    if( stack - stackLimit > size || stack - (char*)&tmp > size ){
        //generate an exception
        tmp = NULL;
        *tmp = 0;
    }
}

#else

#define __DbgCheckCotStackOverflow( dzThread )

#endif // __DBG_CHECK_COT_STACK_OVERFLOW

#ifdef __cplusplus
};
#endif

#endif // __DzBaseWin_h__