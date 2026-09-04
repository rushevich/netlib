#pragma once

#include "netlib/AddressInfo.hpp"
#include "netlib/Common.hpp"
// #include "netlib/Errors.hpp"

#include <cstddef>
#include <memory>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

namespace netlib {

class ResolvedAddresses {
public:
    ResolvedAddresses() = delete;

    /**
     * @brief Constructs an AddressInfoList by performing DNS resolution or parsing local
     * bindings.
     * @param h The hint structure containing socket, family, and protocol preferences.
     * @param hostname The target hostname or IP address string (e.g., "localhost" or
     * "127.0.0.1").
     * @param servname The target service name or port string (e.g., "http" or "8080").
     * @throws std::runtime_error if getaddrinfo fails to resolve the requested address.
     */
    ResolvedAddresses(const Hints& h, const char* hostname = nullptr,
                      const char* servname = nullptr);

    ResolvedAddresses(const ResolvedAddresses&) = delete ("ResolvedAddresses is a move-only type");
    ResolvedAddresses& operator=(const ResolvedAddresses&)
        = delete ("ResolvedAddresses is a move-only type");
    ResolvedAddresses(ResolvedAddresses&& other) noexcept;
    ResolvedAddresses& operator=(ResolvedAddresses&& other) noexcept;

    /**
     * @brief Destructor that automatically frees the underlying linked list from getaddrinfo.
     */
    ~ResolvedAddresses() = default;

    /**
     * @brief Returns the number of resolved network addresses.
     * @return Number of AddressInfo elements in the internal list.
     */
    [[nodiscard]] auto size() const { return _addresses.size(); }

    /**
     * @brief Provides mutable array-style access to the resolved addresses.
     * @param i Index of the desired address.
     * @return Reference to the AddressInfo structure at index i.
     */
    [[nodiscard]] auto& operator[](size_t i) { return _addresses[i]; }

    /**
     * @brief Provides constant array-style access to the resolved addresses.
     * @param i Index of the desired address.
     * @return Const reference to the AddressInfo structure at index i.
     */
    [[nodiscard]] const auto& operator[](size_t i) const { return _addresses[i]; }

private:
    // This is the deleter we will use to avoid writing manual destructors, and maintain RAII
    constexpr static auto addrinfo_deleter = [](addrinfo* ptr) {
        if (ptr != nullptr) {
            freeaddrinfo(ptr);
        }
    };
    using unique_addrinfo_ptr = std::unique_ptr<addrinfo, decltype(addrinfo_deleter)>;

    std::vector<AddressInfo> _addresses;
    unique_addrinfo_ptr _list_ptr { nullptr };
};

} // namespace netlib
