// Copyright (c) 2021 The WhiteBox Authors.  All rights reserved.
// Use of this source code is governed by a 3-Clause BSD license that can be
// found in the LICENSE file.
//
// The entry point for Mac Half-Life 2 process.
//
// Based on
// https://github.com/chromium/chromium/blob/master/chrome/app/chrome_exe_main_mac.cc

#include <dlfcn.h>
#include <libgen.h>
#include <mach-o/dyld.h>
#include <unistd.h>

#include <cerrno>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <system_error>

#include "app_version_config.h"
#include "base/deps/sdl/message_box.h"
#include "base/scoped_shared_library.h"
#include "bootmgr/boot_manager_main.h"

__attribute__((visibility("default"))) int main(int argc, char* argv[]) {
  uint32_t exec_path_size{0};
  int rv{_NSGetExecutablePath(nullptr, &exec_path_size)};
  if (rv != -1) WB_ATTRIBUTE_UNLIKELY {
      wb::sdl::Fatal(WB_PRODUCT_FILE_DESCRIPTION_STRING, std::error_code{rv})
          << "_NSGetExecutablePath: get length failed.  Unable to load the "
             "app.";
    }

  std::unique_ptr<char[]> exec_path{std::make_unique<char[]>(exec_path_size)};
  rv = _NSGetExecutablePath(exec_path.get(), &exec_path_size);
  if (rv != 0) WB_ATTRIBUTE_UNLIKELY {
      wb::sdl::Fatal(WB_PRODUCT_FILE_DESCRIPTION_STRING, std::error_code{rv})
          << "_NSGetExecutablePath: get path failed.  Unable to load the app.";
    }

  constexpr char rel_path[]{
      "../Frameworks/" WB_PRODUCT_NAME_STRING
      " Framework.framework/Versions/" WB_PRODUCT_FILE_VERSION_INFO_STRING
      "/" WB_PRODUCT_FILE_DESCRIPTION_STRING " Framework"};

  // Slice off the last part of the main executable path, and append the
  // version framework information.
  const char* parent_dir{dirname(exec_path.get())};
  if (!parent_dir) WB_ATTRIBUTE_UNLIKELY {
      wb::sdl::Fatal(WB_PRODUCT_FILE_DESCRIPTION_STRING, std::error_code{errno})
          << "dirname '" << exec_path.get() << "'.";
    }

  const size_t parent_dir_len{strlen(parent_dir)};
  const size_t rel_path_len{strlen(rel_path)};
  // 2 accounts for a trailing NUL byte and the '/' in the middle of the paths.
  const size_t framework_path_size{parent_dir_len + rel_path_len + 2};
  std::unique_ptr<char[]> framework_path{
      std::make_unique<char[]>(framework_path_size)};
  snprintf(framework_path.get(), framework_path_size, "%s/%s/%s", parent_dir,
           rel_path,
           "libwhitebox-boot-manager." WB_PRODUCT_VERSION_INFO_STRING ".dylib");

  using namespace wb::base;

  const auto boot_manager_load_result = ScopedSharedLibrary::FromLibraryOnPath(
      framework_path.get(), RTLD_LAZY | RTLD_LOCAL | RTLD_FIRST);
  if (const auto* rc = std2::get_error(boot_manager_load_result))
    WB_ATTRIBUTE_UNLIKELY {
      wb::sdl::Fatal(WB_PRODUCT_FILE_DESCRIPTION_STRING, *rc)
          << "Can't load boot manager '" << framework_path.get() << ".";
    }

  const auto boot_manager_module =
      std::get<ScopedSharedLibrary>(boot_manager_load_result);
  constexpr char kBootManagerMainFunctionName[]{"BootmgrMain"};

  using BootManagerMainFunction = decltype(&BootmgrMain);

  // Good, try to find and launch boot manager.
  const auto boot_manager_entry_result =
      boot_manager_module->GetAddressAs<BootManagerMainFunction>(
          kBootmgrMainFunctionName);
  if (const auto* rc = std2::get_error(bootmgr_entry_result))
    WB_ATTRIBUTE_UNLIKELY {
      wb::sdl::Fatal(WB_PRODUCT_FILE_DESCRIPTION_STRING, *rc)
          << "Can't get '" << kBootmgrMainFunctionName << "' entry point from '"
          << framework_path.get() << "'.";
    }

  const auto boot_manager_main =
      std::get<BootManagerMainFunction>(bootmgr_entry_result);

  rv = boot_manager_main(argc, argv);

  // exit, don't return from main, to avoid the apparent removal of main
  // from stack backtraces under tail call optimization.
  exit(rv);
}