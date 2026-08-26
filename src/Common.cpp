#include "netlib/Common.hpp"

#include <charconv>
#include <cstddef>

namespace netlib {

std::string nbo_to_platform(uint32_t ipv4_addr) {
    static constexpr size_t max_ipv4_addr_size { 16 };
    ipv4_addr = byteorder_ntoh(ipv4_addr);
    std::string result(max_ipv4_addr_size, ' ');
    auto* end { result.data() + max_ipv4_addr_size };

    // Convert 8-bit octets sequentially and insert dot delimiters
    auto ec1 { std::to_chars(result.data(), end, (ipv4_addr >> 24) & 0xFF) };
    *ec1.ptr = '.';
    auto ec2 { std::to_chars(ec1.ptr + 1, end, (ipv4_addr >> 16) & 0xFF) };
    *ec2.ptr = '.';
    auto ec3 { std::to_chars(ec2.ptr + 1, end, (ipv4_addr >> 8) & 0xFF) };
    *ec3.ptr = '.';
    auto ec4 { std::to_chars(ec3.ptr + 1, end, (ipv4_addr >> 0) & 0xFF) };

    // Resize the string to the exact number of characters written to strip trailing spaces
    result.resize(static_cast<size_t>(ec4.ptr - result.data()));

    return result;
}

} // namespace netlib
