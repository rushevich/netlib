#ifndef NETLIB_SOCKET_H
#define NETLIB_SOCKET_H

#include <arpa/inet.h>
#include <bit>
#include <cstdint>
#include <iostream>
#include <netdb.h>
#include <print>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <variant>
#include <vector>

namespace net {

    // Socket type
    inline constexpr int sock_any = 0; // accept any type of
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

} // namespace net

enum class ProtocolFamily : uint8_t { ipv6, ipv4 };
enum class HostType : uint8_t { client, server };
enum class TransportProtocol : uint8_t { tcp, udp };

using px_addrinfo = addrinfo;
using PF_hints = int;
using AI_hints = int;
using TP_hints = int;
using SC_hints = int;

struct Hints {
    SC_hints sock_hints {};   // expects constants from the sock_ category
    AI_hints ainfo_hints {};  // expects constants from the ai_ category
    PF_hints pfam_hints {};   // expects constants from the pf_ category
    TP_hints tproto_hints {}; // expects constants from the ipproto_ category

    /**
     * @brief Casts the internal Hints configuration to a POSIX addrinfo struct.
     * @return A zero-initialized addrinfo struct populated with the specified hints.
     */
    [[nodiscard]] explicit operator addrinfo() const {
        px_addrinfo ainfo {};
        ainfo.ai_socktype = sock_hints;
        ainfo.ai_flags = ainfo_hints;
        ainfo.ai_family = pfam_hints;
        ainfo.ai_protocol = tproto_hints;
        return ainfo;
    }
};

struct AddressInfo {
    int family {};   // ipv6, ipv4 (checked with net::pf_inet and inet6)
    int socktype {}; // stream, dgram (check with net::sock_tcp and udp)
    int protocol {}; // tcp, udp (check with net::ipproto_tcp and udp)
    size_t addrlen {};
    std::string hostname;
    std::variant<sockaddr_in, sockaddr_in6> addr;

    /**
     * @brief Resolves the stored address family integer to a strongly-typed ProtocolFamily enum.
     * @return ProtocolFamily::ipv4 or ProtocolFamily::ipv6 based on the internal family integer.
     */
    [[nodiscard]] auto protocol_family() const {
        return (family == net::pf_inet) ? ProtocolFamily::ipv4 : ProtocolFamily::ipv6;
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
std::string nbo_to_platform(uint32_t ipv4_addr);

class AddressInfoList {
public:
    AddressInfoList() = delete;

    /**
     * @brief Constructs an AddressInfoList by performing DNS resolution or parsing local bindings.
     * @param h The hint structure containing socket, family, and protocol preferences.
     * @param hostname The target hostname or IP address string (e.g., "localhost" or "127.0.0.1").
     * @param servname The target service name or port string (e.g., "http" or "8080").
     * @throws std::runtime_error if getaddrinfo fails to resolve the requested address.
     */
    explicit AddressInfoList(const Hints& h, const char* hostname = nullptr,
                             const char* servname = nullptr);
                             
    /**
     * @brief Destructor that automatically frees the underlying linked list from getaddrinfo.
     */
    ~AddressInfoList() { free_list(); }

    /**
     * @brief Returns the number of resolved network addresses.
     * @return Number of AddressInfo elements in the internal list.
     */
    [[nodiscard]] auto size() const { return m_addresses.size(); }

    /**
     * @brief Provides mutable array-style access to the resolved addresses.
     * @param i Index of the desired address.
     * @return Reference to the AddressInfo structure at index i.
     */
    [[nodiscard]] auto& operator[](size_t i) { return m_addresses[i]; }

    /**
     * @brief Provides constant array-style access to the resolved addresses.
     * @param i Index of the desired address.
     * @return Const reference to the AddressInfo structure at index i.
     */
    [[nodiscard]] const auto& operator[](size_t i) const { return m_addresses[i]; }

    /**
     * @brief Manually frees the POSIX addrinfo linked list to prevent memory leaks.
     * Normally handled automatically by the destructor.
     */
    void free_list();

    AddressInfoList(const AddressInfoList&) = delete;
    AddressInfoList& operator=(const AddressInfoList&) = delete;

    AddressInfoList(AddressInfoList&&) = delete;
    AddressInfoList& operator=(AddressInfoList&&) = delete;

private:
    std::vector<AddressInfo> m_addresses;
    px_addrinfo* result_ptr { nullptr };
};

template <ProtocolFamily PF, HostType HT, TransportProtocol TP> 
class basic_socket {
public:
    /**
     * @brief Constructs a socket using the configured protocol template parameters.
     * @param hostname The target host (null for local binding, as in creating a server socket).
     * @param servname The port or service name to bind/connect to.
     */
    basic_socket(const char* hostname = nullptr, const char* servname = nullptr) {
        const AddressInfoList ainfo_list(m_hints, hostname, servname);
        for (size_t i {}; i < ainfo_list.size(); ++i) {
            const auto& ainfo { ainfo_list[i] };
            if (m_fd = socket(ainfo.family, ainfo.socktype, ainfo.protocol); m_fd == -1) {
                std::println(std::cerr, "socket inst: error");
                continue;
            }
            // Once a socket is successfully opened, break out of the loop
            break; 
        }
        std::println(std::cerr, "Successfully constructed socket.");
    }

    // For now, sockets will not me movable or copyable until we encounter a need for this. Then, these will be implemented
    basic_socket(const basic_socket&) = delete;
    basic_socket& operator=(const basic_socket&) = delete;
    basic_socket(basic_socket&&) = delete;
    basic_socket& operator=(basic_socket&&) = delete;

    /**
     * @brief Closes the underlying file descriptor, terminating the socket connection.
     */
    ~basic_socket() {
        if (m_fd != closed_fd_val) {
            close(m_fd);
        }
    }

private:
    using fd = int;
    static constexpr int closed_fd_val = -1;
    fd m_fd { closed_fd_val };
    AddressInfo m_ainfo {};
    
    // This is the primary mechanism for parametrizing the type of socket opened
    static constexpr Hints m_hints = [] consteval -> Hints {
        Hints h {};
        if constexpr (HT == HostType::server) {
            h.ainfo_hints = net::ai_passive;
        }
        if constexpr (HT == HostType::client) {
            h.ainfo_hints = 0;
        }
        if constexpr (PF == ProtocolFamily::ipv4) {
            h.pfam_hints = net::pf_inet;
        }
        if constexpr (PF == ProtocolFamily::ipv6) {
            h.pfam_hints = net::pf_inet6;
        }
        if constexpr (TP == TransportProtocol::tcp) {
            h.tproto_hints = net::ipproto_tcp;
            h.sock_hints = net::sock_tcp;
        }

        return h;
    }();
};

using TCP_server_socket = basic_socket<ProtocolFamily::ipv4, HostType::server, TransportProtocol::tcp>;

#endif
