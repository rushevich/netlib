#include "netlib/tcp.hpp"

#include "netlib/AddressInfo.hpp"
#include "netlib/Common.hpp" // for hints and flags
#include "netlib/Errors.hpp"
#include "netlib/ResolvedAddresses.hpp"

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
        = ::connect(handle.get(), (sockaddr*)cached_ainfo.data(), cached_ainfo.socklen());
    if (auto err = get_last_error()) {
        return std::unexpected { err };
    }
    return TcpConnection { std::move(handle), std::move(cached_ainfo) };
};

} // namespace netlib
