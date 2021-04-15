// Copyright (c) 2021 The WhiteBox Authors.  All rights reserved.
// Use of this source code is governed by a 3-Clause BSD license that can be
// found in the LICENSE file.
//
// Scoped handler when new operator fails to allocate memory.

#ifndef WHITEBOX_BASE_INCLUDE_WINDOWS_SCOPED_NEW_HANDLER_H_
#define WHITEBOX_BASE_INCLUDE_WINDOWS_SCOPED_NEW_HANDLER_H_
#ifdef _WIN32
#pragma once
#endif

#include <new.h>

#include <cerrno>   // ENOMEM.
#include <cstdlib>  // exit.

#include "base/include/base_macroses.h"
#include "base/include/deps/g3log/g3log.h"

namespace whitebox::base::windows {
/**
 * @brief Changes handler when new operator fails to allocate memory and reverts
 * back when out of scope.  If a user-defined operator new is provided, the new
 * handler functions are not automatically called on failure.
 */
class ScopedNewHandler {
 public:
  /**
   * @brief Sets handler when new operator fails to allocate memory.
   * @param new_handler Handler.
   * @return nothing.
   */
  explicit ScopedNewHandler(_In_ _PNH new_handler) noexcept
      : previous_new_handler_{::_set_new_handler(new_handler)} {}

  WHITEBOX_NO_COPY_MOVE_CTOR_AND_ASSIGNMENT(ScopedNewHandler);

  ~ScopedNewHandler() noexcept {
    (void)::_set_new_handler(previous_new_handler_);
  }

 private:
  const _PNH previous_new_handler_;
};

/**
 * @brief The run-time system retries allocation each time your function returns
 * a nonzero value and fails if your function returns 0.
 * @param memory_size_bytes Memory size to allocate.
 * @return Non-0 to retry allocation, 0 on failure to allocate.
 */
[[noreturn]] inline int DefaultNewFailureHandler(size_t memory_size_bytes) {
  LOG(FATAL) << "Failed to allocate " << memory_size_bytes
             << " memory bytes via new.  Please, ensure you have enough RAM to "
                "run the app.  Stopping the app.";
  // Unreachable.
  std::exit(ENOMEM);
}
}  // namespace whitebox::base::windows

#endif  // !WHITEBOX_BASE_INCLUDE_WINDOWS_SCOPED_NEW_HANDLER_H_
