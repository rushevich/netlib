#pragma once
#include <concepts>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <type_traits>

namespace netlib {
template <int Level, int Name, typename T>
    requires std::is_trivially_copyable_v<T>
struct socket_option {
    using value_type = T;
    static constexpr int level = Level;
    static constexpr int name = Name;
    T value {};
};

template <int Level, int Name> struct bool_option : socket_option<Level, Name, int> {
    constexpr bool_option() = default;
    constexpr bool_option(bool on) : socket_option<Level, Name, int> { on } {}
};
using reuse_address = bool_option<SOL_SOCKET, SO_REUSEADDR>;
using reuse_port = bool_option<SOL_SOCKET, SO_REUSEPORT>;
using keep_alive = bool_option<SOL_SOCKET, SO_KEEPALIVE>;
using no_delay = bool_option<IPPROTO_TCP, TCP_NODELAY>;
using recv_buffer = socket_option<SOL_SOCKET, SO_RCVBUF, int>;
using send_buffer = socket_option<SOL_SOCKET, SO_SNDBUF, int>;
using linger_opt = socket_option<SOL_SOCKET, SO_LINGER, ::linger>;
using recv_timeout = socket_option<SOL_SOCKET, SO_RCVTIMEO, ::timeval>;

template <typename O>
concept SocketOption = requires {
    typename O::value_type;
    { O::level } -> std::convertible_to<int>;
    { O::name } -> std::convertible_to<int>;
};

} // namespace netlib
