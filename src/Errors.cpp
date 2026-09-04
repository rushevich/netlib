#include "netlib/Errors.hpp"

#include <cerrno>
#include <system_error>

namespace netlib {
std::error_code get_last_error() {
    // system_category errors are specifically for errors originating from the operating system.
    // generic_category errors are for errors that can occur anywhere i.e., generically
    // we care about system_category because we are meddling with POSIX APIs, and hence
    // the errors originate from the OS
    // it’s important to note that often times, POSIX error codes nonetheless map to
    // generic_category equivalent ones
    // https://stackoverflow.com/questions/60900224/whats-the-difference-between-generic-category-and-between-system-category
    return { errno, std::system_category() };
}
} // namespace netlib
