#include <netlib/netlib.hpp>
#include <print>

int main() {
    using namespace netlib;
    try {
        // Attempt to create a passive IPv4 TCP socket bound locally on port 8080 (a server socket)
        // Perhaps this is suggestive that we should try to make some more things noexcept
        TCP_server_socket sock { nullptr, "8080" };
    } catch (...) {
        std::println("Caught some error");
    }
    return 0;
}
