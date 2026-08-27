#ifndef NETLIB_SOCKET_H
#define NETLIB_SOCKET_H

#include "netlib/AddressInfo.hpp"
#include "netlib/AddressInfoList.hpp"
#include "netlib/Common.hpp"

#include <cstddef>
#include <functional>
#include <iostream>
#include <print>
#include <sys/socket.h>
#include <unistd.h>

namespace netlib {

template <ProtocolFamily PF, HostType HT, TransportProtocol TP> class basic_socket {
public:
    /**
     * @brief Constructs a socket using the configured protocol template parameters.
     * @param hostname The target host (null for local binding, as in creating a server socket).
     * @param servname The port or service name to bind/connect to.
     */
    explicit basic_socket(const char* hostname = nullptr, const char* servname = nullptr) {
        const AddressInfoList ainfo_list(m_hints, hostname, servname);
        for (size_t i {}; i < ainfo_list.size(); ++i) {
            const auto& ainfo { ainfo_list[i] };
            if (m_fd = socket(ainfo.family(), ainfo.socktype(), ainfo.protocol()); m_fd == -1) {
                std::println(std::cerr, "socket inst: error");
                continue;
            }
            // Once a socket is successfully opened, record it and break out of the loop
            m_ainfo = ainfo;
            break;
        }
        std::println(std::cerr, "Successfully constructed socket.");
    }

    // For now, sockets will not be movable or copyable until we encounter a need for this.
    // Then, these will be implemented
    basic_socket(const basic_socket&) = delete;
    basic_socket& operator=(const basic_socket&) = delete;
    basic_socket(basic_socket&&) = delete;
    basic_socket& operator=(basic_socket&&) = delete;

    /**
     * @brief Closes the underlying file descriptor, terminating the socket connection.
     */
    ~basic_socket() {
        if (m_fd != closed_fd_val) {
            close(m_fd);
        }
    }

private:
    using fd = int;
    static constexpr fd closed_fd_val = -1;
    fd m_fd { closed_fd_val };
    AddressInfo m_ainfo;

    // This is the primary mechanism for parametrizing the type of socket opened
    static constexpr Hints m_hints = std::invoke([] -> Hints {
        Hints h {};
        if constexpr (HT == HostType::server) {
            h.ainfo_hints = ai_passive;
        }
        if constexpr (HT == HostType::client) {
            h.ainfo_hints = 0;
        }
        if constexpr (PF == ProtocolFamily::ipv4) {
            h.pfam_hints = pf_inet;
        }
        if constexpr (PF == ProtocolFamily::ipv6) {
            h.pfam_hints = pf_inet6;
        }
        if constexpr (TP == TransportProtocol::tcp) {
            h.tproto_hints = ipproto_tcp;
            h.sock_hints = sock_tcp;
        }
        if constexpr (TP == TransportProtocol::udp) {
            h.tproto_hints = ipproto_udp;
            h.sock_hints = sock_udp;
        }

        return h;
    });
};

using TCP_server_socket
    = basic_socket<ProtocolFamily::ipv4, HostType::server, TransportProtocol::tcp>;
using TCP_client_socket
    = basic_socket<ProtocolFamily::ipv4, HostType::client, TransportProtocol::tcp>;

} // namespace netlib

#endif
