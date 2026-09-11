#pragma once
#include "netlib/AddressInfo.hpp"
#include "netlib/Common.hpp"
#include "netlib/SocketHandle.hpp"

#include <cstdint>
#include <expected>
#include <optional>
#include <span>
#include <system_error>

// there is a one way friendship TcpListener -> TcpConnection such that the TcpListener can create
// and access fields of a connection properly. This is required for accepting new connections as a
// listener / server

namespace netlib {
namespace detail {
inline constexpr Hints tcp_con_hints = Hints { .sock_hints = flags::sock_tcp,
                                               .ainfo_hints = flags::ai_canonname,
                                               .pfam_hints = flags::pf_inet,
                                               .tproto_hints = flags::ipproto_tcp };

} // namespace detail
class TcpConnection {
private:
    SocketHandle _handle;
    AddressInfo
        _ainfo; // This grows the data structure but it will be useful for debugging/diagnostics
    explicit TcpConnection(SocketHandle&& handle, AddressInfo&& ainfo);

public:
    static std::expected<TcpConnection, std::error_code> connect(const char* host,
                                                                 const char* port);

    // These are quite rough outlines, when it comes to implementing them, we may discover a
    // more ergonomic way to write them in terms of parameters, return types, etc.
    std::optional<std::error_code> send(std::span<const uint8_t>);

    std::optional<std::span<uint8_t>> recv(std::span<uint8_t>);

    TcpConnection(const TcpConnection&) = delete ("TcpConnection is move only.");
    TcpConnection& operator=(const TcpConnection&) = delete ("TcpConnection is move only.");
    ~TcpConnection() = default; // SocketHandler is the only data member and is an RAII type
    TcpConnection(TcpConnection&&) = default;
    TcpConnection& operator=(TcpConnection&&) = default;
};

// The TcpListener is a factory for what is semantically a stream server socket
class TcpListener {
private:
    SocketHandle _handle;
    AddressInfo _ainfo;
    explicit TcpListener(SocketHandle&& handle, AddressInfo&& ainfo)
        : _handle { std::move(handle) },
          _ainfo { std::move(ainfo) } {}

public:
    friend class TcpConnection;
    // Uses the host and port arguments to create a TcpListener / server socket at the host on the
    // port. To create a server for use with the internet, pass in an empty hostname.
    static std::expected<TcpListener, std::error_code> bind(const char* host, const char* port);

    // Accepts any available connections as a TcpConnection
    std::expected<TcpConnection, std::error_code> accept();

    TcpListener(const TcpListener&) = delete ("TcpListener is move only.");
    TcpListener& operator=(const TcpListener&) = delete ("TcpListener is move only.");
    ~TcpListener() = default; // SocketHandler is the only data member and is an RAII type
    TcpListener(TcpListener&&) = default;
    TcpListener& operator=(TcpListener&&) = default;
};
} // namespace netlib
