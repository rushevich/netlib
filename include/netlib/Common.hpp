#pragma once

#include <arpa/inet.h>
#include <bit>
#include <cstdint>
#include <netdb.h>
#include <string>

namespace netlib {

using px_addrinfo = addrinfo;

using PF_hints = int;
using AI_hints = int;
using TP_hints = int;
using SC_hints = int;

// Inline right now because I don't want to go and qualify every use
inline namespace flags {

// Socket type
inline constexpr int sock_any = 0; // accept any type
inline constexpr int sock_tcp = 1; // TCP socket
inline constexpr int sock_udp = 2; // UDP socket

// IP protocols
inline constexpr int ipproto_any = 0; // accept any protocol
inline constexpr int ipproto_tcp = 6;
inline constexpr int ipproto_udp = 17;

// getaddrinfo hint flags
inline constexpr int ai_passive = 1;
inline constexpr int ai_canonname = 2;
inline constexpr int ai_numerichost = 4;
inline constexpr int ai_all = 256;
inline constexpr int ai_v4mapped_cfg = 512;
inline constexpr int ai_addrconfig = 1024;
inline constexpr int ai_v4mapped = 2048;
inline constexpr int ai_numericserv = 4096;
inline constexpr int ai_unusable = 268435456;

// Composite flag sets
inline constexpr int ai_default = ai_v4mapped_cfg | ai_addrconfig; // 1536

// Protocol families
inline constexpr int pf_unspec = 0; // returned addresses can be ipv4 or ipv6
inline constexpr int pf_inet = 2;   // ipv4
inline constexpr int pf_inet6 = 30; // ipv6

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
