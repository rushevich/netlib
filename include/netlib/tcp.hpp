#pragma once
#include "netlib/SocketHandle.hpp"

#include <cstdint>
#include <expected>
#include <optional>
#include <span>
#include <system_error>

namespace netlib {
class TcpConnection {
private:
    SocketHandle _handle;
    explicit TcpConnection(SocketHandle&& handle) : _handle { std::move(handle) } {}

public:
    static std::expected<TcpConnection, std::error_code> connect(const char* host,
                                                                 const char* port);

    // These are quite rough outlines, when it comes to implementing them, we may discover a more
    // ergonomic way to write them in terms of parameters, return types, etc.
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
    explicit TcpListener(SocketHandle&& handle) : _handle { std::move(handle) } {}

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
