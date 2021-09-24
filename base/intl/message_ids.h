// Copyright (c) 2021 The WhiteBox Authors.  All rights reserved.
// Use of this source code is governed by a 3-Clause BSD license that can be
// found in the LICENSE file.
//
// Localized message ids.
//
// TODO(dimhotepus): Autogenerate.

#ifndef WB_BASE_INTL_MESSAGE_IDS_H_
#define WB_BASE_INTL_MESSAGE_IDS_H_

#include <cstdint>

#include "build/build_config.h"

namespace wb::base::intl::message_ids {
#ifdef WB_OS_WIN
// Windows is too old.  At least Windows 10, version 1903 (May 19, 2019)+
// required.
constexpr std::uint64_t kWindowsVersionIsTooOld{1};
// Please, update Windows to Windows 10, version 1903 (May 19, 2019) or greater.
constexpr std::uint64_t kPleaseUpdateWindowsVersion{2};
// See techical details
constexpr std::uint64_t kSeeTechnicalDetails{4};
// Hide techical details
constexpr std::uint64_t kHideTechnicalDetails{5};
#endif
// Boot Manager - Error
constexpr std::uint64_t kBootmgrErrorDialogTitle{3};
// <A HREF=\"https://github.com/The-White-Box/whitebox/issues\">Nudge</A>
// authors
constexpr std::uint64_t kNudgeAuthorsLink{6};
// Can't get current directory.  Unable to load the kernel.
constexpr std::uint64_t kCantGetExecutableDirectoryForBootManager{7};
// Can't get '{0}' entry point from '{1}'.
constexpr std::uint64_t kCantGetLibraryEntryPoint{9};
// Looks like app is broken, please, reinstall the one.
constexpr std::uint64_t kPleaseReinstallTheGame{11};
// Can't load whitebox kernel '{0}'.
constexpr std::uint64_t kCantLoadKernelFrom{12};
// {0} - Error
constexpr std::uint64_t kAppErrorDialogTitle{13};
// Please, check app is installed correctly and you have enough permissions to
// run it.
constexpr std::uint64_t kPleaseCheckAppInstalledCorrectly{14};
// Can't get current directory.  Unable to load the app.
constexpr std::uint64_t kCantGetCurrentDirectoryUnableToLoadTheApp{15};
// Can't load boot manager '{0}'.
constexpr std::uint64_t kCantLoadBootManager{16};
// Whitebox Kernel - Error
constexpr std::uint64_t kKernelErrorDialogTitle{17};
// Please, check mouse is connected and working.
constexpr std::uint64_t kPleaseCheckMouseOnYourDevice{18};
// Unable to register mouse as <A
// HREF=\"https://docs.microsoft.com/en-us/windows/win32/inputdev/about-raw-input\">Raw
// Input</A> device.
constexpr std::uint64_t kUnableToRegisterMouseDevice{19};
// Please, check keybaord is connected and working.
constexpr std::uint64_t kPleaseCheckKeyboardOnYourDevice{20};
// Unable to register keyboard as <A
// HREF=\"https://docs.microsoft.com/en-us/windows/win32/inputdev/about-raw-input\">Raw
// Input</A> device.
constexpr std::uint64_t kUnableToRegisterKeyboardDevice{21};
}  // namespace wb::base::intl::message_ids

#endif  // !WB_BASE_INTL_MESSAGE_IDS_H_
