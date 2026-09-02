#pragma once

#include <arpa/inet.h>
#include <bit>
#include <cstdint>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

namespace netlib {

using px_addrinfo = addrinfo;
using px_sockaddr_storage = sockaddr_storage;

using PF_hints = int;
using AI_hints = int;
using TP_hints = int;
using SC_hints = int;

// Inline right now because I don't want to go and qualify every use
inline namespace flags {

// Socket type
inline constexpr int sock_any = 0;           // accept any type
inline constexpr int sock_tcp = SOCK_STREAM; // TCP socket
inline constexpr int sock_udp = SOCK_DGRAM;  // UDP socket

// IP protocols
inline constexpr int ipproto_any = 0; // accept any protocol
inline constexpr int ipproto_tcp = IPPROTO_TCP;
inline constexpr int ipproto_udp = IPPROTO_UDP;

// getaddrinfo hint flags
inline constexpr int ai_passive = AI_PASSIVE;
inline constexpr int ai_canonname = AI_CANONNAME;
inline constexpr int ai_numerichost = AI_NUMERICHOST;
inline constexpr int ai_all = AI_ALL;
// inline constexpr int ai_v4mapped_cfg = AI_V4MAPPEDCFG;
inline constexpr int ai_addrconfig = AI_ADDRCONFIG;
inline constexpr int ai_v4mapped = AI_V4MAPPED;
inline constexpr int ai_numericserv = AI_NUMERICSERV;
// inline constexpr int ai_unusable = AI_UNUSABLE;

// Composite flag sets
// inline constexpr int ai_default = AI_DEFAULT; // 1536

// Protocol families
inline constexpr int pf_unspec = PF_UNSPEC; // returned addresses can be ipv4 or ipv6
inline constexpr int pf_inet = PF_INET;     // ipv4
inline constexpr int pf_inet6 = PF_INET6;   // ipv6

} // namespace flags

enum class ProtocolFamily : uint8_t { ipv6, ipv4 };
enum class HostType : uint8_t { client, server };
enum class TransportProtocol : uint8_t { tcp, udp };

/**
 * @brief Aggregate of the four hint categories accepted by getaddrinfo.
 */
struct Hints {
    SC_hints sock_hints {};   // expects constants from the sock_ category
    AI_hints ainfo_hints {};  // expects constants from the ai_ category
    PF_hints pfam_hints {};   // expects constants from the pf_ category
    TP_hints tproto_hints {}; // expects constants from the ipproto_ category

    /**
     * @brief Casts the Hints configuration to a POSIX addrinfo struct.
     * @return A zero-initialized addrinfo struct populated with the specified hints.
     */
    [[nodiscard]] explicit operator px_addrinfo() const {
        px_addrinfo ainfo {};
        ainfo.ai_socktype = sock_hints;
        ainfo.ai_flags = ainfo_hints;
        ainfo.ai_family = pfam_hints;
        ainfo.ai_protocol = tproto_hints;
        return ainfo;
    }
};

/**
 * @brief Converts a 32-bit integer from Network Byte Order (Big Endian) to Host Byte Order.
 * @param addr The 32-bit address in network byte order.
 * @return The address translated to the native byte order of the host system.
 */
constexpr uint32_t byteorder_ntoh(uint32_t addr) {
    if constexpr (std::endian::native == std::endian::little) {
        return std::byteswap(addr);
    }
    return addr;
}

/**
 * @brief Converts a 32-bit IPv4 address into a standard dot-decimal string representation.
 * @param ipv4_addr The 32-bit address in network byte order.
 * @return A string containing the formatted IPv4 address (e.g., "192.168.1.1").
 */
[[nodiscard]] std::string nbo_to_platform(uint32_t ipv4_addr);

} // namespace netlib
