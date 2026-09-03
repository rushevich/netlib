#include "netlib/AddressInfoList.hpp"

#include <stdexcept>
#include <vector>

namespace netlib {

AddressInfoList::AddressInfoList(const Hints& h, const char* hostname, const char* servname) {
    px_addrinfo hints { static_cast<px_addrinfo>(h) };

    int status { getaddrinfo(hostname, servname, &hints, &result_ptr) };
    if (status != 0) {
        throw std::runtime_error("failed to get addresses");
    }

    for (auto* current_ainfo { result_ptr }; current_ainfo != nullptr;
         current_ainfo = current_ainfo->ai_next) {
        m_addresses.emplace_back(current_ainfo);
    }
}

AddressInfoList::~AddressInfoList() {
    if (result_ptr != nullptr) {
        freeaddrinfo(result_ptr);
    }
}

} // namespace netlib
