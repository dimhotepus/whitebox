// Copyright (c) 2021 The WhiteBox Authors.  All rights reserved.
// Use of this source code is governed by a 3-Clause BSD license that can be
// found in the LICENSE file.
//
// Boot manager main entry point.

#ifndef WB_BOOT_MANAGER_BOOT_MANAGER_MAIN_H_
#define WB_BOOT_MANAGER_BOOT_MANAGER_MAIN_H_

#include <cstddef>  // std::byte
#include <string_view>

#include "base/deps/g3log/g3log.h"
#include "base/intl/lookup_with_fallback.h"
#include "build/build_config.h"
#include "command_line_flags.h"
#include "config.h"

#ifdef WB_OS_WIN
/**
 * @brief HINSTANCE type.
 */
using HINSTANCE = struct HINSTANCE__ *;
#endif

namespace wb::boot_manager {

/**
 * @brief Boot manager args.
 */
struct BootManagerArgs {
#ifdef WB_OS_WIN
  BootManagerArgs(HINSTANCE instance_, std::string_view app_description_,
                  int show_window_flags_, int main_icon_id_, int small_icon_id_,
                  const CommandLineFlags &command_line_flags_,
                  const wb::base::intl::LookupWithFallback &intl_)
      : instance{instance_},
        app_description{app_description_},
        show_window_flags{show_window_flags_},
        main_icon_id{main_icon_id_},
        small_icon_id{small_icon_id_},
        command_line_flags{command_line_flags_},
        intl{intl_} {
    G3DCHECK(!!instance);
  }
#else
  BootManagerArgs(std::string_view app_description_,
                  const CommandLineFlags &command_line_flags_,
                  const wb::base::intl::LookupWithFallback &intl_)
      : app_description{app_description_},
        command_line_flags{command_line_flags_},
        intl{intl_} {}
#endif

#ifdef WB_OS_WIN
  /**
   * @brief App instance.
   */
  HINSTANCE instance;
#endif

  /**
   * @brief App description.
   */
  std::string_view app_description;

#ifdef WB_OS_WIN
  /**
   * @brief Show app window flags.
   */
  int show_window_flags;

  /**
   * @brief Main app icon id.
   */
  int main_icon_id;
  /**
   * @brief Small app icon id.
   */
  int small_icon_id;

  WB_ATTRIBUTE_UNUSED_FIELD std::byte
      pad_[sizeof(char *) - sizeof(small_icon_id)];
#endif

  /**
   * @brief Command line flags.
   */
  const CommandLineFlags &command_line_flags;

  /**
   * @brief Localization service.
   */
  const wb::base::intl::LookupWithFallback &intl;

  WB_NO_COPY_MOVE_CTOR_AND_ASSIGNMENT(BootManagerArgs);
};

}  // namespace wb::boot_manager

/**
 * @brief Boot manager entry point on Windows.
 * @param boot_manager_args Boot manager args.
 * @return 0 on success.
 */
extern "C" [[nodiscard]] WB_BOOT_MANAGER_API int BootManagerMain(
    const wb::boot_manager::BootManagerArgs &boot_manager_args);

#endif  // !WB_BOOT_MANAGER_BOOT_MANAGER_MAIN_H_