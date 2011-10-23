/********************************************************************
    created:    2010/02/11 22:13
    file:       DzResourceMgr.c
    author:     Foreverflying
    purpose:    
*********************************************************************/

#include "DzIncOs.h"
#include "DzBaseOs.h"
#include "DzResourceMgr.h"
#include "DzIoOs.h"
#include "DzSynObj.h"
#include "DzCoreOs.h"
#include "DzCore.h"

BOOL MemeryPoolGrow( DzHost* host )
{
    char* pool;
    DzLNode* node;

    pool = (char*)PageReserv( MEMERY_POOL_GROW_SIZE );
    if( !pool ){
        return FALSE;
    }
    host->memPoolPos = pool;
    host->memPoolEnd = pool + MEMERY_POOL_GROW_SIZE;
    node = AllocLNode( host );
    node->d1 = (intptr_t)pool;
    node->d2 = MEMERY_POOL_GROW_SIZE;
    node->lItr.next = host->poolGrowList;
    host->poolGrowList = &node->lItr;
    return TRUE;
}

void ReleaseMemoryPool( DzHost* host )
{
    DzLNode* node;
    void** p;
    int* len;
    int count = 0;
    int poolGrowCount = 0;
    DzLItr* lItr = host->poolGrowList;

    while( lItr ){
        poolGrowCount++;
        lItr = lItr->next;
    }
    if( !poolGrowCount ){
        return;
    }
    p = (void**)alloca( sizeof(void*) * poolGrowCount );
    len = (int*)alloca( sizeof(int) * poolGrowCount );
    lItr = host->poolGrowList;
    while( lItr ){
        node = MEMBER_BASE( lItr, DzLNode, lItr );
        *(p + count) = (void*)node->d1;
        *(len + count++) = (int)node->d2;
        lItr = lItr->next;
    }
    for( count = 0; count < poolGrowCount; count++ ){
        PageFree( p[ count ], len[ count ] );
    }
}

BOOL AllocListNodePool( DzHost* host )
{
    DzLNode* p;
    DzLNode* end;
    DzLNode* tmp;
    DzLItr* lItr;

    p = (DzLNode*)AllocChunk( host, OBJ_POOL_GROW_COUNT * sizeof( DzLNode ) );
    if( !p ){
        return FALSE;
    }

    host->lNodePool = &p->lItr;
    end = p + OBJ_POOL_GROW_COUNT - 1;
    end->lItr.next = NULL;
    tmp = p;
    while( tmp != end ){
        lItr = &tmp->lItr;
        lItr->next = &(++tmp)->lItr;
    }
    return TRUE;
}

BOOL AllocSynObjPool( DzHost* host )
{
    DzSynObj* p;
    DzSynObj* end;
    DzLItr* lItr;

    p = (DzSynObj*)AllocChunk( host, OBJ_POOL_GROW_COUNT * sizeof( DzSynObj ) );
    if( !p ){
        return FALSE;
    }

    host->synObjPool = &p->lItr;
    end = p + OBJ_POOL_GROW_COUNT - 1;
    end->lItr.next = NULL;
    end->ref = 0;
    InitDList( &end->waitQ[ CP_HIGH ] );
    InitDList( &end->waitQ[ CP_NORMAL ] );
    InitDList( &end->waitQ[ CP_LOW ] );
    while( p != end ){
        p->ref = 0;
        InitDList( &p->waitQ[ CP_HIGH ] );
        InitDList( &p->waitQ[ CP_NORMAL ] );
        InitDList( &p->waitQ[ CP_LOW ] );
        lItr = &p->lItr;
        lItr->next = &(++p)->lItr;
    }
    return TRUE;
}

BOOL AllocDzCotPool( DzHost* host )
{
    DzCot* p;
    DzCot* end;
    DzLItr* lItr;

    p = (DzCot*)AllocChunk( host, OBJ_POOL_GROW_COUNT * sizeof( DzCot ) );
    if( !p ){
        return FALSE;
    }

    host->cotPool = &p->lItr;
    end = p + OBJ_POOL_GROW_COUNT - 1;
    end->lItr.next = NULL;
    InitDzCot( host, end );
    while( p != end ){
        InitDzCot( host, p );
        lItr = &p->lItr;
        lItr->next = &(++p)->lItr;
    }
    return TRUE;
}
