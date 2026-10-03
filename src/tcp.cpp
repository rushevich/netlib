#include "netlib/tcp.hpp"

#include "netlib/AddressInfo.hpp"
#include "netlib/Errors.hpp"
#include "netlib/ResolvedAddresses.hpp"

#include <sys/socket.h>

namespace netlib {
TcpConnection::TcpConnection(SocketHandle&& handle, AddressInfo&& ainfo)
    : _handle { std::move(handle) },
      _ainfo { std::move(ainfo) } {}

std::expected<TcpConnection, std::error_code> TcpConnection::connect(const char* host,
                                                                     const char* port) {
    auto addrs = ResolvedAddresses { detail::tcp_con_hints, host, port };

    int cached_fd {};
    AddressInfo cached_ainfo;
    for (size_t i {}; i < addrs.size(); i++) {
        auto& ainfo { addrs[i] };
        int fd = ::socket(ainfo.family(), ainfo.socktype(), ainfo.protocol());
        // std::error_code contextually converts to bool, and if its 0 there’s no issue
        if (get_last_error()) {
            continue;
        }
        cached_ainfo = std::move(ainfo);
        cached_fd = fd;
        break; // Break because it means the socket was successfully created.
    }
    SocketHandle handle { cached_fd };
    [[maybe_unused]] auto placeholder
        = ::connect(handle.get(), reinterpret_cast<const sockaddr*>(cached_ainfo.data()),
                    cached_ainfo.socklen());
    if (auto err = get_last_error()) {
        return std::unexpected { err };
    }
    return TcpConnection { std::move(handle), std::move(cached_ainfo) };
};

// ---------------------- TcpListener members ----------------------------
std::expected<TcpListener, std::error_code> TcpListener::bind(const char* port, const char* host) {
    auto addrs = ResolvedAddresses { detail::tcp_listener_hints, host, port };

    int cached_fd {};
    AddressInfo cached_ainfo;
    for (size_t i {}; i < addrs.size(); i++) {
        auto& ainfo { addrs[i] };
        int fd = ::socket(ainfo.family(), ainfo.socktype(), ainfo.protocol());
        // std::error_code contextually converts to bool, and if its 0 there’s no issue
        if (get_last_error()) {
            continue;
        }
        cached_ainfo = std::move(ainfo);
        cached_fd = fd;
        break; // Break because it means the socket was successfully created.
    }
    SocketHandle handle { cached_fd };
    [[maybe_unused]] auto placeholder
        = ::bind(handle.get(), reinterpret_cast<sockaddr const*>(cached_ainfo.data()),
                 cached_ainfo.socklen());
    if (auto err = get_last_error()) {
        return std::unexpected { err };
    }
    return TcpListener { std::move(handle), std::move(cached_ainfo) };
};

std::expected<TcpConnection, std::error_code> TcpListener::accept() {
    sockaddr newAddr {};
    socklen_t newLen {};
    int newFd {};
    if (newFd = ::accept(_fd(), &newAddr, &newLen); newFd == -1) {
        return std::unexpected { get_last_error() };
    }
    return TcpConnection { SocketHandle { newFd }, AddressInfo {} };
}
} // namespace netlib
