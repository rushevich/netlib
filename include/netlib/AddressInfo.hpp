#ifndef NETLIB_AINFO_H
#define NETLIB_AINFO_H

#include "netlib/Common.hpp"

#include <arpa/inet.h>
#include <string>
#include <variant>

namespace netlib {

struct AddressInfo {
    int family {};   // ipv6, ipv4 (checked with pf_inet and inet6)
    int socktype {}; // stream, dgram (check with sock_tcp and udp)
    int protocol {}; // tcp, udp (check with ipproto_tcp and udp)
    std::string hostname;
    std::variant<sockaddr_in, sockaddr_in6> addr;

    /**
     * @brief Resolves the stored address family integer to a strongly-typed ProtocolFamily
     * enum.
     * @return ProtocolFamily::ipv4 or ProtocolFamily::ipv6 based on the internal family
     * integer.
     */
    [[nodiscard]] auto protocol_family() const {
        return (family == pf_inet) ? ProtocolFamily::ipv4 : ProtocolFamily::ipv6;
    }

    /**
     * @brief Retrieves a mutable reference to the underlying IPv4 socket address structure.
     * @throws std::bad_variant_access if the address is not IPv4.
     * @return Reference to sockaddr_in.
     */
    [[nodiscard]] auto& ipv4_addr() { return std::get<sockaddr_in>(addr); }

    /**
     * @brief Retrieves a constant reference to the underlying IPv4 socket address structure.
     * @throws std::bad_variant_access if the address is not IPv4.
     * @return Const reference to sockaddr_in.
     */
    [[nodiscard]] const auto& ipv4_addr() const { return std::get<sockaddr_in>(addr); }

    /**
     * @brief Retrieves a mutable reference to the underlying IPv6 socket address structure.
     * @throws std::bad_variant_access if the address is not IPv6.
     * @return Reference to sockaddr_in6.
     */
    [[nodiscard]] auto& ipv6_addr() { return std::get<sockaddr_in6>(addr); }

    /**
     * @brief Retrieves a constant reference to the underlying IPv6 socket address structure.
     * @throws std::bad_variant_access if the address is not IPv6.
     * @return Const reference to sockaddr_in6.
     */
    [[nodiscard]] const auto& ipv6_addr() const { return std::get<sockaddr_in6>(addr); }
};

} // namespace netlib

#endif
