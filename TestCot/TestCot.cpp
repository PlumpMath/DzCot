// TestCot.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <gtest/gtest.h>
#include "TestCompileInC.h"

int main(int argc, _TCHAR* argv[])
{
    TestCompileInC();
    char* av[] = {
        "TestCot",
        "--gtest_break_on_failure",
        //"--gtest_filter=TestCotMechanism.*",
        //"--gtest_filter=TestSynObj.*",
        //"--gtest_filter=TestCallbackTimer.*",
        //"--gtest_filter=TestException.*",
        //"--gtest_filter=TestFileReadWrite.*",
        "--gtest_filter=TestTcpSocket.*",
        //"--gtest_filter=TestTcpSocket.Simple*",
        //"--gtest_filter=TestTcpSocket.SimpleSend",
        //"--gtest_filter=TestTcpSocket.SimpleRecv",
        //"--gtest_filter=TestTcpSocket.SendRecvSendRecv",
        //"--gtest_filter=TestTcpSocket.LargeBuffer",
        ""
    };
    int ac = sizeof( av ) / sizeof( char* );
    testing::InitGoogleTest( &ac, av );

    //testing::InitGoogleTest( &argc, argv );
    int ret = RUN_ALL_TESTS();
    getchar();
    return ret;
}

int _tmain(int argc, _TCHAR* argv[])
{
    return main( argc, argv );
}
