#include "netlib/ResolvedAddresses.hpp"

#include <memory>
#include <stdexcept>
#include <vector>

namespace netlib {

ResolvedAddresses::ResolvedAddresses(const Hints& h, const char* hostname, const char* servname) {
    px_addrinfo hints { static_cast<px_addrinfo>(h) };
    addrinfo* dest_ptr { nullptr };
    int status { getaddrinfo(hostname, servname, &hints, &dest_ptr) };
    _list_ptr.reset(dest_ptr); // claims ownership of the linkedlist

    // https://isocpp.org/wiki/faq/exceptions#ctors-can-throw
    if (status != 0) {
        throw std::runtime_error("No addresses resolved!");
    }

    for (auto* current_ainfo { _list_ptr.get() }; current_ainfo != nullptr;
         current_ainfo = current_ainfo->ai_next) {
        _addresses.emplace_back(current_ainfo);
    }
}

ResolvedAddresses::ResolvedAddresses(ResolvedAddresses&& other) noexcept
    : _addresses(std::move(other._addresses)) {
    _list_ptr.reset(other._list_ptr.get());
    other._list_ptr = nullptr;
}
ResolvedAddresses& ResolvedAddresses::operator=(ResolvedAddresses&& other) noexcept {
    if (&other != this) {
        std::swap(_addresses, other._addresses);
        _list_ptr.reset(other._list_ptr.get());
        other._list_ptr = nullptr;
    }
    return *this;
}

} // namespace netlib
