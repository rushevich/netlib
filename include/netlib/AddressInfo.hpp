#pragma once

#include "netlib/Common.hpp"

#include <arpa/inet.h>
#include <cstring> // for std::mem*
#include <string>
#include <sys/socket.h>

namespace netlib {
// Can either be ipv6 or ipv4 addressinfo
class AddressInfo {
public:
    /**
     * @brief Constructs a new address info from a posix addrinfo pointer (straight from the
     * linkedlist returned by getaddrinfo)
     */
    AddressInfo() = default;
    explicit AddressInfo(px_addrinfo* ainfo)
        : /*_addr { (px_sockaddr_storage*)
                      ainfo->ai_addr }, // we have to be careful and make sure to use this before
                                        // freeaddrinfo destroys the linked list
                                        */
          _hostname { ainfo->ai_canonname == nullptr ? "" : ainfo->ai_canonname },
          _family { ainfo->ai_family },
          _socktype { ainfo->ai_socktype },
          _protocol { ainfo->ai_protocol } {
        std::memset(
            static_cast<void*>(&_addr), 0,
            sizeof(
                px_sockaddr_storage)); // It is best practice to 0 memory out before copying to it
        std::memcpy(static_cast<void*>(&_addr), ainfo->ai_addr, ainfo->ai_addrlen);
        // We copy the memory to prevent losing it when ainfo gets freed
    }

    [[nodiscard]] auto data() const { return &_addr; };

    [[nodiscard]] auto socklen() const {
        if (_family == pf_inet) {
            return sizeof(sockaddr_in);
        }
        return sizeof(sockaddr_in6);
    };

    [[nodiscard]] auto family() const { return _family; }

    [[nodiscard]] auto socktype() const { return _socktype; }

    [[nodiscard]] auto protocol() const { return _protocol; }

    [[nodiscard]] auto hostname() const { return _hostname; }

private:
    px_sockaddr_storage _addr {}; // storage that works with both inet4 and inet6
                                  // we can always cast back based on the stored _family
    std::string _hostname;        // human-readable
    int _family {};               // ipv6, ipv4 (checked with pf_inet and inet6)
    int _socktype {};             // stream, dgram (check with sock_tcp and udp)
    int _protocol {};             // tcp, udp (check with ipproto_tcp and udp)
};

} // namespace netlib
