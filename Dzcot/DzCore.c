/********************************************************************
    created:    2010/02/11 22:11
    file:       DzCore.c
    author:     Foreverflying
    purpose:    
*********************************************************************/

#include "DzIncOs.h"
#include "DzCore.h"
#include "DzIoOs.h"

void __stdcall DelayFreeCotHelper( intptr_t context )
{
    DzHost* host = GetHost();
    DzCot* dzCot = (DzCot*)context;

    FreeCotStack( dzCot );
    dzCot->lItr.next = host->cotPool;
    host->cotPool = &dzCot->lItr;
}

void __stdcall EventNotifyCotEntry( intptr_t context )
{
    DzHost* host = GetHost();
    DzCotParam* param = (DzCotParam*)context;

    param->entry( param->context );
    SetEvt( host, param->evt );
    CloseSynObj( host, param->evt );
    FreeLNode( host, (DzLNode*)param );
}

void __stdcall CallbackTimerEntry( intptr_t context )
{
    DzSynObj* timer = (DzSynObj*)context;

    if( timer->routine ){
        timer->routine( timer->context );
    }
    timer->ref--;
    if( timer->ref == 0 ){
        InitDList( &timer->waitQ[ CP_HIGH ] );
        InitDList( &timer->waitQ[ CP_NORMAL ] );
        FreeSynObj( GetHost(), timer );
    }
}

// CotScheduleCenter:
// the Cot Schedule Center uses the host's origin thread's stack
// manager all kernel objects that may cause real block
void CotScheduleCenter( DzHost* host )
{
    int n;

    while( 1 ){
        while( 1 ){
            if( host->scheduleCd ){
                n = DispatchMinTimers( host );
                n = DispatchRmtCots( host, n );
                if( n == 0 ){
                    host->currPri = CP_FIRST;
                    Schedule( host );
                    continue;
                }else if( !host->cotCount ){
                    break;
                }
            }else{
                n = 0;
            }
            BlockAndDispatchIo( host, n );
            DispatchMinTimers( host );
            DispatchRmtCots( host, 0 );
            host->currPri = CP_FIRST;
            host->scheduleCd = SCHEDULE_COUNTDOWN;
            Schedule( host );
        }
        if( host->lazyTimer ){
            DealLazyResEntry( 0 );
        }
        if( AtomAndInt( &host->hostMgr->exitSign, ~host->hostMask ) != host->hostMask ){
            BlockAndDispatchIo( host, -1 );
            if( AtomReadInt( &host->hostMgr->exitSign ) ){
                AtomAndInt( &host->checkRmtSign, ~RMT_CHECK_SLEEP_SIGN );
                continue;
            }
        }else{
            //be sure quit id 0 host at the end, for hostMgr is in id 0 host's stack
            for( n = host->hostMgr->hostCount - 1; n >= 0; n-- ){
                if( host->hostMgr->hostArr[ n ] && n != host->hostId ){
                    AwakeRemoteHost( host->hostMgr->hostArr[ n ] );
                }
            }
        }
        break;
    }
}
