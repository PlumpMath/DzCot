
#include "CotTestUtil.h"

static int gCount = 0;
static int helpCalledCount = 0;

CotEntry CallbackTimerRoutine( intptr_t context )
{
    gCount++;
}

CotEntry HelpTestCallback( intptr_t context )
{
    DzHandle timer = (DzHandle)context;

    DzDelCallbackTimer( timer );
}

CotEntry TestOneOffCallbackTimer( intptr_t context )
{
    gCount = 0;
    DzHandle timer = DzCreateCallbackTimer( 500, FALSE, CallbackTimerRoutine );

    DzSleep( 200 );
    DZ_EXPECT_EQ( 0, gCount );
    DzSleep( 500 );
    DZ_EXPECT_EQ( 1, gCount );
    DzSleep( 100 );
    DZ_EXPECT_EQ( 1, gCount );
    DzSleep( 500 );
    DZ_EXPECT_EQ( 1, gCount );

    DzDelCallbackTimer( timer );
}

CotEntry TestRepeatCallbackTimer( intptr_t context )
{
    gCount = 0;
    DzHandle timer = DzCreateCallbackTimer( 500, TRUE, CallbackTimerRoutine );

    DzSleep( 800 );
    DZ_EXPECT_EQ( 1, gCount );
    DzSleep( 800 );
    DZ_EXPECT_EQ( 3, gCount );
    DzSleep( 500 );
    DZ_EXPECT_EQ( 4, gCount );
    DzSleep( 500 );
    DZ_EXPECT_EQ( 5, gCount );
    DzSleep( 500 );
    DZ_EXPECT_EQ( 6, gCount );

    DzDelCallbackTimer( timer );

    DzSleep( 500 );
    DZ_EXPECT_EQ( 6, gCount );
}

CotEntry TestEnsureTimerCancelAfterClose( intptr_t context )
{
    gCount = 0;
    DzHandle timer = DzCreateCallbackTimer( 500, TRUE, CallbackTimerRoutine, 0, CP_NORMAL );

    DzSleep( 500 );
    DZ_EXPECT_EQ( 1, gCount );
    DzSleep( 500 );
    DZ_EXPECT_EQ( 2, gCount );

    helpCalledCount = 0;
    DzHandle helpTimer = DzCreateCallbackTimer( 500, FALSE, HelpTestCallback, (intptr_t)timer, CP_HIGH );

    DzSleep( 500 );
    DZ_EXPECT_EQ( 2, gCount );

    DzDelCallbackTimer( helpTimer );
}

TEST( TestCallbackTimer, OneOffCallbackTimer )
{
    TestCot( TestOneOffCallbackTimer );
}

TEST( TestCallbackTimer, RepeatCallbackTimer )
{
    TestCot( TestRepeatCallbackTimer );
}

TEST( TestCallbackTimer, EnsureTimerCancelAfterClose )
{
    TestCot( TestEnsureTimerCancelAfterClose );
}