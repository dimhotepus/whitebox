// Copyright (c) 2021 The WhiteBox Authors.  All rights reserved.
// Use of this source code is governed by a 3-Clause BSD license that can be
// found in the LICENSE file.
//
// App main window on *nix.

#ifndef WB_KERNEL_MAIN_WINDOW_POSIX_H_
#define WB_KERNEL_MAIN_WINDOW_POSIX_H_

#include <string_view>
#include <utility>  // std::move

#include "base/deps/sdl/window.h"
#include "base/intl/lookup_with_fallback.h"
#include "base/macroses.h"

namespace wb::kernel {

/**
 * Main window via SDL.
 */
class MainWindow {
 public:
  /**
   * @brief Create main window.
   * @param title Title.
   * @param width Width.
   * @param height Height.
   * @param window_flags Window flags.
   * @param intl Localization lookup.
   * @return Main window.
   */
  [[nodiscard]] static sdl::result<MainWindow> New(
      std::string_view title, int width, int height,
      sdl::WindowFlags window_flags,
      const base::intl::LookupWithFallback& intl) noexcept;

  /**
   * Move constructor.
   * @param w Window to move construct from.
   */
  MainWindow(MainWindow&& w) noexcept : window_{std::move(w.window_)} {}

  /**
   * Move assignment operator.
   * @param w Window to move assign from.
   */
  MainWindow& operator=(MainWindow&& w) noexcept {
    std::swap(window_, w.window_);
    return *this;
  }

  WB_NO_COPY_CTOR_AND_ASSIGNMENT(MainWindow);

 private:
  /**
   * SDL window.
   */
  sdl::Window window_;

  /**
   * Creates main window.
   * @param window SDL window.
   */
  explicit MainWindow(sdl::Window&& window) noexcept
      : window_{std::move(window)} {}
};

}  // namespace wb::kernel

#endif  // !WB_KERNEL_MAIN_WINDOW_POSIX_H_
