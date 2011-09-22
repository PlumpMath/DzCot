// TestCot.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <gtest/gtest.h>

int _tmain(int argc, _TCHAR* argv[])
{
    _TCHAR* av[] = {
        _T("TestCot"),
        //_T("--gtest_break_on_failure"),
        //_T("--gtest_filter=*"),
        //_T("--gtest_filter=TestSynObj.*"),
        //_T("--gtest_filter=TestCallbackTimer.*"),
        _T("--gtest_filter=TestSocket.*"),
        //_T("--gtest_filter=TestSocket.SendRecvSend"),
        _T("")
    };
    int ac = sizeof( av ) / sizeof(_TCHAR*);

    testing::InitGoogleTest( &ac, av );

    int ret = RUN_ALL_TESTS();
    getchar();
    return ret;
}