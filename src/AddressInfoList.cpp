#include "netlib/AddressInfoList.hpp"

#include <stdexcept>
#include <variant>

namespace netlib {

AddressInfoList::AddressInfoList(const Hints& h, const char* hostname, const char* servname) {
    px_addrinfo hints { static_cast<px_addrinfo>(h) };

    int status { getaddrinfo(hostname, servname, &hints, &result_ptr) };
    if (status != 0) {
        throw std::runtime_error("failed to get addresses");
    }

    for (auto* current_ainfo { result_ptr }; current_ainfo != nullptr;
         current_ainfo = current_ainfo->ai_next) {
        std::variant<sockaddr_in, sockaddr_in6> addr;

        if (current_ainfo->ai_family == pf_inet) {
            addr = *reinterpret_cast<sockaddr_in*>(current_ainfo->ai_addr);
        } else {
            addr = *reinterpret_cast<sockaddr_in6*>(current_ainfo->ai_addr);
        }

        m_addresses.emplace_back(
            current_ainfo->ai_family, current_ainfo->ai_socktype, current_ainfo->ai_protocol,
            (current_ainfo->ai_canonname == nullptr) ? "" : current_ainfo->ai_canonname, addr);
    }
}

void AddressInfoList::free_list() {
    if (result_ptr != nullptr) {
        // Simply use the POSIX cleanup here
        // It would be nice if we could call without the conditional, however calling
        // freeaddrinfo on a nullptr is UB
        freeaddrinfo(result_ptr);
        result_ptr = nullptr;
    }
}

} // namespace netlib
