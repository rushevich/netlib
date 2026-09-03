#pragma once

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
    // TODO: constrain the arguments that can be logically passed into the constructor depending on
    // whether the socket is client or server
    /**
     * @brief Constructs a socket using the configured protocol template parameters.
     * @param hostname The target host (null for local binding, as in creating a server socket).
     * @param servname The port or service name to bind/connect to.
     */
    explicit basic_socket(const char* hostname = nullptr, const char* port = nullptr) {
        { // scope so that the list is destroyed after we discover a good address
            const AddressInfoList ainfo_list(m_hints, hostname, port);
            for (size_t i {}; i < ainfo_list.size(); ++i) {
                const auto& ainfo { ainfo_list[i] };
                if (_fd = socket(ainfo.family(), ainfo.socktype(), ainfo.protocol());
                    _fd == error_val) {
                    std::println(std::cerr, "socket inst: error");
                    continue;
                }
                // Once a socket is successfully opened, record it and break out of the loop
                _ainfo = ainfo;
                break;
            }
#ifndef NDEBUG
            std::println(std::cerr, "Successfully constructed socket with hostname: {}",
                         _ainfo.hostname());
#endif

            // Only servers are required to bind
            if constexpr (HT == HostType::server) {
                if (auto err = bind(_fd, (sockaddr*)_ainfo.data(), _ainfo.socklen()); err != 0) {
                    throw std::runtime_error("failed to bind socket");
                }
            } else if constexpr (HT == HostType::client) {
                if (auto err = connect(_fd, (sockaddr*)_ainfo.data(), _ainfo.socklen()); err != 0) {
                    throw std::runtime_error("failed to connect socket");
                }
            }
        } // addressinfolist is destroyed here since we no longer need the address info
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
        if (_fd != closed_fd_val) {
            close(_fd);
        }
    }

private:
    using fd = int;
    fd _fd { closed_fd_val };
    AddressInfo _ainfo;

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

    // Symbolic constants
    static constexpr fd closed_fd_val = -1;
    static constexpr int error_val = -1;
};

using TCP_server_socket
    = basic_socket<ProtocolFamily::ipv4, HostType::server, TransportProtocol::tcp>;
using TCP_client_socket
    = basic_socket<ProtocolFamily::ipv4, HostType::client, TransportProtocol::tcp>;

} // namespace netlib
