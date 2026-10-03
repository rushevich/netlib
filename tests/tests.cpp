#include <gtest/gtest.h>
#include <netlib/netlib.hpp>
#include <utility>
using namespace netlib;

TEST(TcpTests, TcpClientConnect) {
    auto facebook_client = TcpConnection::connect("facebook.com", "80");
    ASSERT_TRUE(facebook_client.has_value());
}

TEST(TcpTests, TcpServerBind) {
    auto silly_server = TcpListener::bind("10000");
    ASSERT_TRUE(silly_server.has_value());
}

TEST(TcpTests, TcpServerAccept) {
    auto exp = TcpListener::bind("8080");
    ASSERT_TRUE(exp.has_value());
    auto server = std::move(exp.value());
    [[maybe_unused]] auto other = [] {
        auto exp = TcpConnection::connect("127.0.0.1", "8080");
        ASSERT_TRUE(exp.has_value());
    };
}
