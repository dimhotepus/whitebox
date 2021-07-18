// Copyright (c) 2021 The WhiteBox Authors.  All rights reserved.
// Use of this source code is governed by a 3-Clause BSD license that can be
// found in the LICENSE file.
//
// Thread extensions.

#ifndef WB_BASE_THREADS_THREAD_UTILS_H_
#define WB_BASE_THREADS_THREAD_UTILS_H_

#include <string>
#include <string_view>
#include <thread>

#include "base/base_api.h"
#include "base/base_macroses.h"
#include "base/deps/g3log/g3log.h"
#include "build/compiler_config.h"

#ifdef WB_OS_WIN
#include <sal.h>
#endif

namespace wb::base::threads {
/**
 * @brief Native thread handle.
 */
using ThreadHandle = std::thread::native_handle_type;

/**
 * @brief Gets thread name.
 * @param handle Thread handle.
 * @param thread_name Thread name.
 * @return Error code.
 */
[[nodiscard]] WB_BASE_API std::error_code GetThreadName(
    _In_ ThreadHandle handle, _Out_ std::string& thread_name);

/**
 * @brief Set thread name.
 * @param handle Thread handle.
 * @param thread_name New thread name.
 * @return Error code.
 */
[[nodiscard]] WB_BASE_API std::error_code SetThreadName(
    _In_ ThreadHandle handle, _In_ const std::string &thread_name);

/**
 * @brief Scoped thread name.
 */
class ScopedThreadName {
 public:
  /**
   * @brief Set name for thread in scope and restore out of scope.
   * @param thread Thread.
   * @param new_thread_name Scoped thread name.
   */
  explicit ScopedThreadName(_In_ ThreadHandle thread,
                            _In_ const std::string &new_thread_name)
      : thread_{thread}, error_code_{GetThreadName(thread, old_thread_name_)} {
    G3CHECK(!error_code());

    if (!error_code()) error_code_ = SetThreadName(thread_, new_thread_name);
  }

  WB_NO_COPY_MOVE_CTOR_AND_ASSIGNMENT(ScopedThreadName);

  /**
   * @brief Restore previous thread name.
   */
  ~ScopedThreadName() noexcept {
    if (!error_code()) G3CHECK(!SetThreadName(thread_, old_thread_name_));
  }

  /**
   * @brief Get error code.
   * @return Error code.
   */
  [[nodiscard]] std::error_code error_code() const noexcept {
    return error_code_;
  }

 private:
  /**
   * @brief Thread handle.
   */
  const ThreadHandle thread_;
  /**
   * @brief Error code.
   */
  std::error_code error_code_;
  /**
   * @brief Previous thread name.
   */
  std::string old_thread_name_;
};
}  // namespace wb::base::threads

#endif  // !WB_BASE_THREADS_THREAD_UTILS_H_
