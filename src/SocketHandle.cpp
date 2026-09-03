#include "netlib/SocketHandle.hpp"

#include <unistd.h> // For close()

namespace netlib {
SocketHandle& SocketHandle::operator=(SocketHandle&& other) noexcept {
    if (this != &other) {
        close();
        // This could have some implications.
        // The alternative is to have some FD registry where we make sure all are closed before
        // quitting. But this is additional overhead not unlike garbage collection
        _fd = other._fd;
        other._fd = invalid_fd; // Prevents the other, moved-from SocketHandle from being
                                // improperly destroyed
    }
    return *this;
}

SocketHandle::~SocketHandle() noexcept {
    if (_fd != invalid_fd) {
        ::close(static_cast<int>(_fd));
    }
}

void SocketHandle::close() {
    if (_fd != invalid_fd) {
        ::close(static_cast<int>(_fd));
        _fd = invalid_fd;
    }
}
} // namespace netlib
