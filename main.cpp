#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <print>
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
    inline constexpr int ai_passive
        = 1; // returned socket address structure is intended for use with bind (i.e., for servers
             // that wish to use a local address and port to listen for and accept incoming
             // connections)
             // if not set, the socket address is intended to be used as a "client" i.e., to connect
             // and send. if hostname is null and ai_passive is not set, the socket addr will be set
             // to the loopback address
    inline constexpr int ai_canonname
        = 2; // if set & getaddrinfo call is successful, returns canonical hostname in canonname
             // field of first addrinfo struct in the list
    inline constexpr int ai_numerichost = 4; // treat hostname as numeric string identifying the
                                             // ipv4/6 address and don't perform hostname resolution
    inline constexpr int ai_all = 256; // returns all matching addresses if ai_v4mapped is also set
    inline constexpr int ai_v4mapped_cfg
        = 512; // behaves exactly like ai_v4mapped if kernel supports ipv4-mapped ipv6 addresses
    inline constexpr int ai_addrconfig
        = 1024; // only returns the addresses for which the system is configured (i.e., only return
                // ipv4 if ipv4 is configured)
    inline constexpr int ai_v4mapped = 2048; // if configged with pf_inet6, returns ipv4 mapped ipv6
                                             // addrs. ignored if pf is not ipv6
    inline constexpr int ai_numericserv
        = 4096; // non-null servname is treated as numeric port number
    inline constexpr int ai_unusable
        = 268435456; // to suppress the ai_default setting that occurs when no flags are passed.
                     // this inhibits filtering of unusable addresses from the returned list

    // Composite flag sets
    inline constexpr int ai_default = ai_v4mapped_cfg | ai_addrconfig; // 1536

    // Protocol families
    inline constexpr int pf_unspec = 0; // returned addresses can be ipv4 or ipv6
    inline constexpr int pf_inet = 2;   // ipv4
    inline constexpr int pf_inet6 = 30; // ipv6

} // namespace net

enum class ProtocolFamily : uint8_t { ipv6, ipv4 };

enum class HostType : uint8_t { client, server };

enum class TransportProtocol : uint8_t {
    tcp,
    udp,
};
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

    [[nodiscard]] explicit operator addrinfo() const {
        px_addrinfo ainfo {};
        ainfo.ai_socktype = sock_hints;
        ainfo.ai_flags = ainfo_hints;
        ainfo.ai_family = pfam_hints;
        ainfo.ai_protocol = tproto_hints;
        return ainfo;
    }
};

// struct sockaddr_in {          // IPv4
//     sa_family_t    sin_family;
//     in_port_t      sin_port;  // network byte order (uint16_t)
//     struct in_addr sin_addr; // this is just uint32_t sin_addr
// };

// struct sockaddr_in6 {         // IPv6
//     sa_family_t     sin6_family;
//     in_port_t       sin6_port;
//     uint32_t        sin6_flowinfo;
//     struct in6_addr sin6_addr; // this is a uint8_t array of size 16
//     uint32_t        sin6_scope_id;
// };

struct AddressInfo {
    int family {};   // ipv6, ipv4 (checked with net::pf_inet and inet6)
    int socktype {}; // stream, dgram (check with net::sock_tcp and udp)
    int protocol {}; // tcp, udp (check with net::ipproto_tcp and udp)
    size_t addrlen {};
    std::string hostname;
    std::variant<sockaddr_in, sockaddr_in6> addr;

    [[nodiscard]] auto protocol_family() const {
        return (family == net::pf_inet) ? ProtocolFamily::ipv4 : ProtocolFamily::ipv6;
    }
    [[nodiscard]] auto& ipv4_addr() { return std::get<sockaddr_in>(addr); }
    [[nodiscard]] const auto& ipv4_addr() const { return std::get<sockaddr_in>(addr); }
    [[nodiscard]] auto& ipv6_addr() { return std::get<sockaddr_in6>(addr); }
    [[nodiscard]] const auto& ipv6_addr() const { return std::get<sockaddr_in6>(addr); }
};

#include <bit>
constexpr uint32_t byteorder_ntoh(uint32_t addr) {
    if constexpr (std::endian::native == std::endian::little) {
        return std::byteswap(addr);
    }
    return addr;
}

#include <charconv>
// akin to the Posix-C inet_ntop()
// TODO: Handle the error codes properly
auto nbo_to_platform(uint32_t ipv4_addr) {
    static constexpr size_t max_ipv4_addr_size { 16 };
    ipv4_addr = byteorder_ntoh(ipv4_addr);
    std::string result(max_ipv4_addr_size, ' ');
    auto* end { result.data() + max_ipv4_addr_size };
    auto ec1 = std::to_chars(result.data(), end, (ipv4_addr >> 24) & 0xFF);
    *ec1.ptr = '.';
    auto ec2 = std::to_chars(ec1.ptr + 1, end, (ipv4_addr >> 16) & 0xFF);
    *ec2.ptr = '.';
    auto ec3 { std::to_chars(ec2.ptr + 1, end, (ipv4_addr >> 8) & 0xFF) };
    *ec3.ptr = '.';
    auto ec4 { std::to_chars(ec3.ptr + 1, end, (ipv4_addr >> 0) & 0xFF) };

    return result;
}

class AddressInfoList {
public:
    AddressInfoList() = delete;
    explicit AddressInfoList(const Hints& h, const char* hostname = nullptr,
                             const char* servname = nullptr) { // very non-trivial
        px_addrinfo hints { static_cast<px_addrinfo>(h) };
        int status { getaddrinfo(hostname, servname, &hints, &result_ptr) };
        if (status != 0) {
            throw std::runtime_error("failed to get addresses");
        }
        for (auto* current_ainfo { result_ptr }; current_ainfo != nullptr;
             current_ainfo = current_ainfo->ai_next) {
            std::variant<sockaddr_in, sockaddr_in6> addr;
            if (current_ainfo->ai_family == net::pf_inet) {
                addr = *reinterpret_cast<sockaddr_in*>(current_ainfo->ai_addr);
            } else {
                addr = *reinterpret_cast<sockaddr_in6*>(current_ainfo->ai_addr);
            }
            m_addresses.emplace_back(
                current_ainfo->ai_family, current_ainfo->ai_socktype, current_ainfo->ai_protocol,
                current_ainfo->ai_addrlen,
                (current_ainfo->ai_canonname == nullptr) ? "" : current_ainfo->ai_canonname, addr);
        }
    }
    [[nodiscard]] auto size() const { return m_addresses.size(); }
    [[nodiscard]] auto& operator[](size_t i) { return m_addresses[i]; }
    [[nodiscard]] const auto& operator[](size_t i) const { return m_addresses[i]; }

    auto free_list() {
        if (result_ptr != nullptr) {
            freeaddrinfo(result_ptr);
            result_ptr = nullptr;
        }
    }
    ~AddressInfoList() { free_list(); }

    AddressInfoList(const AddressInfoList&) = delete;
    AddressInfoList& operator=(const AddressInfoList&) = delete;

    AddressInfoList(AddressInfoList&&) = delete;
    AddressInfoList& operator=(AddressInfoList&&) = delete;

private:
    std::vector<AddressInfo> m_addresses;
    px_addrinfo* result_ptr { nullptr };
};

// TODO:
// - define/declare which instantiations have access to bind, connect, listen, accept
template <ProtocolFamily PF, HostType HT, TransportProtocol TP> class basic_socket {
public:
    // For construction, it is appropriate to just scope this to
    basic_socket(const char* hostname = nullptr, const char* servname = nullptr) {
        const AddressInfoList ainfo_list(m_hints, hostname, servname);
        for (size_t i {}; i < ainfo_list.size(); ++i) {
            const auto& ainfo { ainfo_list[i] };
            if (m_fd = socket(ainfo.family, ainfo.socktype, ainfo.protocol); m_fd == -1) {
                std::println(std::cerr, "socket inst: error");
                continue;
            }
        }
        std::println(std::cerr, "Successfully constructed socket.");
        // std::println(std::cerr, )
    }

    basic_socket(const basic_socket&) = delete;
    basic_socket& operator=(const basic_socket&) = delete;
    basic_socket(basic_socket&&) = delete;
    basic_socket& operator=(basic_socket&&) = delete;

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
            h.tproto_hints = net::sock_tcp;
        }

        return h;
    }();
};

using TCP_server_socket
    = basic_socket<ProtocolFamily::ipv4, HostType::server, TransportProtocol::tcp>;

int main(int argc, char* argv[]) {
    try {
        TCP_server_socket sock { nullptr, "8080" };
    } catch (...) {
        std::println("Caught some error");
    }
    return 0;
}
