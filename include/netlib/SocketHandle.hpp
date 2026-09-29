#pragma once

#include "netlib/Errors.hpp"
#include "netlib/SocketOption.hpp"

#include <compare> // spaceship operator
#include <expected>
#include <sys/socket.h>
#include <system_error>
namespace netlib {

// Strongly-typed PosixFD with an explicit conversion operator
// to its underlying integer representation.
// associated with the socket.
struct PosixFD {
public:
    PosixFD() = default;
    constexpr explicit PosixFD(int fd) : _fd { fd } {}

    explicit operator int() const { return _fd; }

    auto operator<=>(const PosixFD& other) const = default;

private:
    int _fd {};
};

// SocketHandle manages a raw socket file descriptor that is
// provided by the POSIX socket API. The purpose of this class
// is to be composed by various different types / applications
// of sockets and manage the closing of the file descriptor
class SocketHandle {
public:
    SocketHandle() = default;
    explicit SocketHandle(int fd) : _fd { fd } {}

    ~SocketHandle() noexcept;

    SocketHandle(const SocketHandle&) = delete ("SocketHandle is a move-only type");
    SocketHandle& operator=(const SocketHandle&) = delete ("SocketHandle is a move-only type");

    SocketHandle(SocketHandle&& other) noexcept : _fd { other._fd } { other._fd = invalid_fd; }
    SocketHandle& operator=(SocketHandle&& other) noexcept;

    void close();

    // Accessor for the raw FD
    [[nodiscard]] int get() const { return static_cast<int>(_fd); }

    template <SocketOption O> std::expected<void, std::error_code> setOpt(O opt) {
        if (::setsockopt(_fd, opt.level, opt.name, opt.value, sizeof opt.value)) {
            return std::unexpected { get_last_error() };
        }
        return {};
    }

private:
    PosixFD _fd { invalid_fd };
    static constexpr auto invalid_fd = PosixFD { -1 };
};
} // namespace netlib
