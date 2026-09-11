#include <iostream>
#include <netlib/netlib.hpp>
#include <print>
int main() {
    using namespace netlib;
    // Attempt to create a passive IPv4 TCP socket bound locally on port 8080 (a server socket)
    // Perhaps this is suggestive that we should try to make some more things noexcept
    // TCP_server_socket sock { nullptr, "8080" };
    auto facebook_client = TcpConnection::connect("facebook.com", "80");
    if (facebook_client.has_value()) {
        std::println(std::cerr, "Success!!!");
    } else {
        std::println(std::cerr, "FUCKKKK!!!");
    }

    auto silly_server = TcpListener::bind("10000");
    if (silly_server.has_value()) {
        std::println(std::cerr, "Success!!! Again");
    } else {
        std::println(std::cerr, "Http server socket error: {}", silly_server.error().message());
    }

    return 0;
}
