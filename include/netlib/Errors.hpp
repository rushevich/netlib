#pragma once

#include <system_error>

namespace netlib {
  // Function that gets the last error code from the system (via ‘errno’) and
  // returns a properly mapped std::error_code
  // Note that std::error_code is platform specific. Later on, we can write an implementation
  // using the portable std::error_condition, but for now we can just target POSIX
  std::error_code get_last_error();
}
