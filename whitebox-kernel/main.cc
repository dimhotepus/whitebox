// Copyright (c) 2021 The WhiteBox Authors.  All rights reserved.
// Use of this source code is governed by a 3-Clause BSD license that can be
// found in the LICENSE file.
//
// Whitebox kernel main entry point.

#include "main.h"

#include "base/deps/fmt/core.h"
#include "base/deps/g3log/g3log.h"
#include "whitebox-ui/fatal_dialog.h"

#ifdef WB_OS_WIN
#include "base/win/windows_light.h"
#include "whitebox-kernel/main_window_win.h"
#include "whitebox-ui/win/base_window.h"
#include "whitebox-ui/win/peek_message_dispatcher.h"
#else
#include <chrono>
#include <thread>

#include "base/deps/sdl/cursor.h"
#include "base/deps/sdl/init.h"
#include "base/deps/sdl/version.h"
#include "base/deps/sdl/window.h"
#include "base/deps/sdl_image/init.h"
#include "build/static_settings_config.h"
#endif

namespace {

#ifdef WB_OS_WIN
/**
 * @brief Creates main app window definition.
 * @param kernel_args Kernel args.
 * @param window_title Window title.
 * @param width Window width.
 * @param height WIndow height.
 * @return Window definition.
 */
[[nodiscard]] wb::ui::win::WindowDefinition CreateMainWindowDefinition(
    const wb::kernel::KernelArgs& kernel_args, const char* window_title,
    _In_ int width, _In_ int height) noexcept {
  G3DCHECK(!!kernel_args.instance);

  // NOLINTNEXTLINE(performance-no-int-to-ptr): System header define.
  const auto cursor = LoadCursor(nullptr, IDC_ARROW);
  return wb::ui::win::WindowDefinition{
      kernel_args.instance, window_title, kernel_args.main_icon_id,
      kernel_args.small_icon_id, cursor,
      // TODO(dimhotepus): Remove when use Vulkan renderer?
      reinterpret_cast<HBRUSH>(GetStockObject(NULL_BRUSH)), WS_OVERLAPPEDWINDOW,
      0, CW_USEDEFAULT, CW_USEDEFAULT, width, height};
}

/**
 * @brief Run app message loop.
 * @return App exit code.
 */
[[nodiscard]] int DispatchMessages(
    _In_z_ const char* main_window_name) noexcept {
  int rc{0};
  bool is_done{false};
  const auto handle_quit_message = [&](const MSG& msg) noexcept {
    if (msg.message == WM_QUIT) {
      rc = static_cast<int>(msg.wParam);
      is_done = true;
    }
  };

  using namespace wb::ui::win;
  PeekMessageDispatcher message_dispatcher;

  // Main message app loop.
  // NOLINTNEXTLINE(bugprone-infinite-loop): Loop ends in dispatcher.
  while (!is_done) {
    message_dispatcher.Dispatch(HasNoPreDispatchMessage, handle_quit_message);
  }

  G3LOG_IF(WARNING, rc != 0)
      << "Main window '" << main_window_name
      << "' message dispatch thread exited with non success code " << rc;

  return rc;
}
#else
/**
 * @brief Run app message loop.
 * @return App exit code.
 */
[[nodiscard]] int DispatchMessages() noexcept {
  SDL_Event event;
  bool is_done{false};

  while (!is_done) {
    while (::SDL_PollEvent(&event) == 1) {
      switch (event.type) {
        case SDL_QUIT:
          is_done = true;
          break;

        default:
          continue;
      }
    }

    // TODO(dimhotepus): Do smth when no events.
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(5ms);
  }

  return 0;
}

/**
 * @brief Get used by window graphics context.
 * @param flags SdlWindowFlags.
 * @return Window graphics context.
 */
[[nodiscard]] constexpr const char* GetWindowGraphicsContext(
    wb::sdl::SdlWindowFlags flags) noexcept {
  if ((flags & wb::sdl::SdlWindowFlags::kUseOpengl) ==
      wb::sdl::SdlWindowFlags::kUseOpengl) {
    return "OpenGL";
  }

  if ((flags & wb::sdl::SdlWindowFlags::kUseVulkan) ==
      wb::sdl::SdlWindowFlags::kUseVulkan) {
    return "Vulkan";
  }

  if ((flags & wb::sdl::SdlWindowFlags::kUseMetal) ==
      wb::sdl::SdlWindowFlags::kUseMetal) {
    return "Metal";
  }

  return "N/A";
}
#endif

/**
 * @brief Makes fatal dialog context.
 * @param kernel_args Kernel arguments.
 * @return Fatal dialog context.
 */
[[nodiscard]] wb::ui::FatalDialogContext MakeFatalContext(
    const wb::kernel::KernelArgs& kernel_args) noexcept {
#ifdef WB_OS_POSIX
  return wb::ui::FatalDialogContext{kernel_args.intl.Layout()};
#elif defined(WB_OS_WIN)
  return {kernel_args.intl, kernel_args.intl.Layout(), kernel_args.main_icon_id,
          kernel_args.small_icon_id};
#else
#error "Please define MakeFatalContext for your platform."
#endif
}

}  // namespace

extern "C" [[nodiscard]] WB_WHITEBOX_KERNEL_API int KernelMain(
    const wb::kernel::KernelArgs& kernel_args) {
  using namespace wb::base;
  using namespace wb::kernel;

  // TODO(dimhotepus): Get screen size and use it if less than our minimal.
  constexpr int window_width{1024};
  constexpr int window_height{768};

  const auto& intl = kernel_args.intl;

#ifdef WB_OS_WIN
  const wb::ui::win::WindowDefinition window_definition{
      CreateMainWindowDefinition(kernel_args, kernel_args.app_description,
                                 window_width, window_height)};
  constexpr DWORD window_class_style{CS_HREDRAW | CS_VREDRAW};

  auto window_result = wb::ui::win::BaseWindow::New<MainWindow>(
      window_definition, window_class_style, intl);
  if (auto* window_ptr = std2::get_result(window_result);
      auto* window = window_ptr ? window_ptr->get() : nullptr) {
    // If the window was previously visible, the return value is nonzero.  If
    // the window was previously hidden, the return value is zero.
    window->Show(kernel_args.show_window_flags);
    // Send WM_PAINT directly to draw first time.
    window->Update();

    return DispatchMessages(window_definition.name);
  }

  wb::ui::FatalDialog(
      intl::l18n_fmt(intl, "{0} - Error", kernel_args.app_description),
      std::get<std::error_code>(window_result),
      intl::l18n_fmt(intl,
                     "Please, check app is installed correctly and you have "
                     "enough permissions to run it."),
      MakeFatalContext(kernel_args),
      intl::l18n_fmt(intl, "Unable to create main '{0}' window.",
                     window_definition.name));
#else
  using namespace wb::sdl;
  using namespace wb::sdl_image;

  const ::SDL_version compiled_sdl_version{GetCompileTimeVersion()},
      linked_sdl_version{GetLinkTimeVersion()};
  const auto sdl_initializer = SdlInitializer::New(SdlInitializerFlags::kAudio |
                                                   SdlInitializerFlags::kVideo);
  if (const auto* error = get_error(sdl_initializer)) WB_ATTRIBUTE_UNLIKELY {
      wb::ui::FatalDialog(
          intl::l18n_fmt(intl, "{0} - Error", kernel_args.app_description), {},
          intl::l18n(intl,
                     "Please, check your SDL library installed and working."),
          MakeFatalContext(kernel_args),
          intl::l18n_fmt(intl,
                         "SDL build/runtime v.{0}/v.{1}, revision '{2}' "
                         "initialization failed.\n\n{3}.",
                         compiled_sdl_version, linked_sdl_version,
                         ::SDL_GetRevision(), *error));
    }

  // Try to use wait cursor while window is created.  Should go after SDL init.
  un<ScopedSdlCursor> set_wait_cursor_while_app_starts{
      CreateScopedCursor(SdlSystemCursor::kWaitArrow)};
  G3LOG(INFO) << "SDL versions: build " << compiled_sdl_version << ", runtime "
              << linked_sdl_version << '.';

  constexpr SdlImageInitializerFlags sdl_image_initializer_flags{
      SdlImageInitializerFlags::kJpg | SdlImageInitializerFlags::kPng};
  const auto sdl_image_initializer =
      SdlImageInitializer::New(sdl_image_initializer_flags);
  if (const auto* error = get_error(sdl_image_initializer))
    WB_ATTRIBUTE_UNLIKELY {
      wb::ui::FatalDialog(
          intl::l18n_fmt(intl, "{0} - Error", kernel_args.app_description), {},
          intl::l18n(intl,
                     "Please, check your SDL library installed and working."),
          MakeFatalContext(kernel_args),
          intl::l18n_fmt(intl,
                         "SDL image parser initialization failed for image "
                         "types {0}.\n\n{1}.",
                         sdl_image_initializer_flags, *error));
    }

  // TODO(dimhotepus): kAllowHighDpi handling at least on Mac.
  const SdlWindowFlags window_flags {
    SdlWindowFlags::kResizable | SdlWindowFlags::kAllowHighDpi
#if defined(WB_OS_LINUX)
        | SdlWindowFlags::kUseVulkan
#elif defined(WB_OS_MACOS)
        | SdlWindowFlags::kUseMetal
#else
#error "Please, define SDL window flags for your platform."
#endif
  };

  const auto sdl_window = SdlWindow::New(
      kernel_args.app_description, SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, window_width, window_height, window_flags);
  const auto* window = get_result(sdl_window);
  if (!window) WB_ATTRIBUTE_UNLIKELY {
      wb::ui::FatalDialog(
          intl::l18n_fmt(intl, "{0} - Error", kernel_args.app_description), {},
          intl::l18n_fmt(intl,
                         "Please, check you installed '{0}' libraries/drivers.",
                         GetWindowGraphicsContext(window_flags)),
          MakeFatalContext(kernel_args),
          intl::l18n_fmt(
              intl, "SDL window create failed with '{0}' context.\n\n{1}.",
              GetWindowGraphicsContext(window_flags), *get_error(sdl_window)));
    }

  const std::string window_icon_name{std::string{kernel_args.app_description} +
                                     " icon.png"};
  const auto window_icon_result =
      SdlSurface::FromImage(window_icon_name.c_str());
  if (const auto* window_icon = get_result(window_icon_result))
    WB_ATTRIBUTE_LIKELY { window->SetIcon(*window_icon); }
  else {
    const auto* error = get_error(window_icon_result);
    G3LOG(WARNING) << "SDL unable to set window icon to " << window_icon_name
                   << ", run with default one: " << *error << ".";
  }

  window->SetMinimumSizes(
      wb::build::settings::ui::window::dimensions::kMinWidth,
      wb::build::settings::ui::window::dimensions::kMinHeight);

  {
    G3LOG(INFO) << "SDL graphics context: "
                << GetWindowGraphicsContext(window_flags) << ".";

    SDL_SysWMinfo platform_window_info;
    const auto rc = window->GetPlatformInfo(platform_window_info);

    G3LOG(INFO) << "SDL window subsystem: "
                << (rc.is_succeeded() ? platform_window_info.subsystem
                                      : ::SDL_SYSWM_UNKNOWN)
                << ".";
  }

  // Startup sequence finished, window is already shown, restore default cursor.
  set_wait_cursor_while_app_starts.reset();

  return DispatchMessages();
#endif
}