#include <gtest/gtest.h>
#include <netlib/netlib.hpp>
#include <thread>
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
    auto bindResult = TcpListener::bind("8080");
    ASSERT_TRUE(bindResult.has_value());
    auto server = std::move(bindResult.value());
    [[maybe_unused]] auto otherThread = [] {
        auto exp = TcpConnection::connect("127.0.0.1", "8080"); // 127.0.0.1 is loopback
        ASSERT_TRUE(exp.has_value());
    };
    std::jthread clientThread { otherThread };
    auto acceptResult = server.accept();
    ASSERT_TRUE(acceptResult.has_value());
    auto clientEnd = std::move(acceptResult.value());
}
