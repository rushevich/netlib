#ifndef NETLIB_AINFOLIST_H
#define NETLIB_AINFOLIST_H

#include "netlib/AddressInfo.hpp"
#include "netlib/Common.hpp"

#include <cstddef>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

namespace netlib {

class AddressInfoList {
public:
    AddressInfoList() = delete;

    /**
     * @brief Constructs an AddressInfoList by performing DNS resolution or parsing local
     * bindings.
     * @param h The hint structure containing socket, family, and protocol preferences.
     * @param hostname The target hostname or IP address string (e.g., "localhost" or
     * "127.0.0.1").
     * @param servname The target service name or port string (e.g., "http" or "8080").
     * @throws std::runtime_error if getaddrinfo fails to resolve the requested address.
     */
    AddressInfoList(const Hints& h, const char* hostname = nullptr,
                             const char* servname = nullptr);

    AddressInfoList(const AddressInfoList&) = delete;
    AddressInfoList& operator=(const AddressInfoList&) = delete;
    AddressInfoList(AddressInfoList&&) = delete;
    AddressInfoList& operator=(AddressInfoList&&) = delete;

    /**
     * @brief Destructor that automatically frees the underlying linked list from getaddrinfo.
     */
    ~AddressInfoList();

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

private:
    std::vector<AddressInfo> m_addresses;
    px_addrinfo* result_ptr { nullptr };
};

} // namespace netlib

#endif
