// Copyright (c) 2021 The WhiteBox Authors.  All rights reserved.
// Use of this source code is governed by a 3-Clause BSD license that can be
// found in the LICENSE file.
//
// Scoped handler when new operator fails to allocate memory.

#ifndef WHITEBOX_BASE_INCLUDE_SCOPED_NEW_HANDLER_H_
#define WHITEBOX_BASE_INCLUDE_SCOPED_NEW_HANDLER_H_
#ifdef _WIN32
#pragma once
#endif

#include <cerrno>   // ENOMEM.
#include <cstdlib>  // exit.
#include <new>

#include "base_macroses.h"
#include "deps/g3log/g3log.h"

namespace whitebox::base {
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
  explicit ScopedNewHandler(_In_ std::new_handler new_handler) noexcept
      : previous_new_handler_{std::set_new_handler(new_handler)} {}

  WHITEBOX_NO_COPY_MOVE_CTOR_AND_ASSIGNMENT(ScopedNewHandler);

  ~ScopedNewHandler() noexcept { std::set_new_handler(previous_new_handler_); }

 private:
  const std::new_handler previous_new_handler_;
};

/**
 * @brief Default new failure handler.
 * @return void.
 */
[[noreturn]] inline void DefaultNewFailureHandler() {
  LOG(FATAL) << "Failed to allocate memory bytes via new.  Please, ensure you "
                "have enough RAM to run the app.  Stopping the app.";
  // Unreachable.
  std::exit(ENOMEM);
};
}
}  // namespace whitebox::base

#endif  // !WHITEBOX_BASE_INCLUDE_WINDOWS_NEW_HANDLER_H_
