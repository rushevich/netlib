#include <gtest/gtest.h>
#include <iostream>
#include <netlib/netlib.hpp>
#include <print>
using namespace netlib;

TEST(TcpTests, TcpClientConnect) {
    auto facebook_client = TcpConnection::connect("facebook.com", "80");
    ASSERT_TRUE(facebook_client.has_value());
}

TEST(TcpTests, TcpServerBind) {
    auto silly_server = TcpListener::bind("10000");
    ASSERT_TRUE(silly_server.has_value());
}
