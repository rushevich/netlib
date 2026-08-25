#include "socket.hpp"
#include <charconv>

std::string nbo_to_platform(uint32_t ipv4_addr) {
    static constexpr size_t max_ipv4_addr_size { 16 };
    ipv4_addr = byteorder_ntoh(ipv4_addr);
    std::string result(max_ipv4_addr_size, ' ');
    auto* end { result.data() + max_ipv4_addr_size };
    
    // Convert 8-bit octets sequentially and insert dot delimiters
    auto ec1 = std::to_chars(result.data(), end, (ipv4_addr >> 24) & 0xFF);
    *ec1.ptr = '.';
    auto ec2 = std::to_chars(ec1.ptr + 1, end, (ipv4_addr >> 16) & 0xFF);
    *ec2.ptr = '.';
    auto ec3 { std::to_chars(ec2.ptr + 1, end, (ipv4_addr >> 8) & 0xFF) };
    *ec3.ptr = '.';
    auto ec4 { std::to_chars(ec3.ptr + 1, end, (ipv4_addr >> 0) & 0xFF) };

    return result;
}

AddressInfoList::AddressInfoList(const Hints& h, const char* hostname, const char* servname) {
    px_addrinfo hints { static_cast<px_addrinfo>(h) };
    
    // Call POSIX getaddrinfo to populate a linked list at result_ptr
    int status { getaddrinfo(hostname, servname, &hints, &result_ptr) };
    if (status != 0) {
        throw std::runtime_error("failed to get addresses");
    }
    
    // Iterate over the resulting linked list to populate our local std::vector
    for (auto* current_ainfo { result_ptr }; current_ainfo != nullptr;
         current_ainfo = current_ainfo->ai_next) {
        std::variant<sockaddr_in, sockaddr_in6> addr;
        
        // Cast raw sockaddr to appropriate family structure based on ai_family
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

void AddressInfoList::free_list() {
    if (result_ptr != nullptr) {
        // Simply use the POSIX cleanup here
	// It would be nice if we could call without the conditional, however calling freeaddrinfo on a nullptr is UB
        freeaddrinfo(result_ptr);
        result_ptr = nullptr;
    }
}
