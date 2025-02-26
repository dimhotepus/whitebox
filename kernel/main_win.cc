// Copyright (c) 2021 The WhiteBox Authors.  All rights reserved.
// Use of this source code is governed by a 3-Clause BSD license that can be
// found in the LICENSE file.
//
// Whitebox kernel main entry point.

#include "base/deps/fmt/core.h"
#include "base/deps/g3log/g3log.h"
#include "base/high_resolution_clock.h"
#include "base/intl/l18n.h"
#include "base/win/windows_light.h"
#include "kernel/input/input_queue.h"
#include "kernel/main_simulate_step.h"
#include "kernel/main_window_win.h"
#include "main.h"
#include "ui/fatal_dialog.h"
#include "ui/win/base_window.h"
#include "ui/win/peek_message_dispatcher.h"

namespace {

/**
 * @brief Creates main app window definition.
 * @param kernel_args Kernel args.
 * @param window_title Window title.
 * @param width Window width.
 * @param height WIndow height.
 * @return Window definition.
 */
[[nodiscard]] wb::ui::win::WindowDefinition CreateMainWindowDefinition(
    const wb::kernel::KernelArgs& kernel_args, std::string_view window_title,
    _In_ int width, _In_ int height) noexcept {
  G3DCHECK(!!kernel_args.instance);

  // NOLINTNEXTLINE(performance-no-int-to-ptr): System header define.
  const auto cursor = LoadCursor(nullptr, IDC_ARROW);
  return wb::ui::win::WindowDefinition{kernel_args.instance,
                                       window_title,
                                       kernel_args.main_icon_id,
                                       kernel_args.small_icon_id,
                                       cursor,
                                       nullptr,
                                       WS_OVERLAPPEDWINDOW,
                                       0,
                                       CW_USEDEFAULT,
                                       CW_USEDEFAULT,
                                       width,
                                       height};
}

/**
 * @brief Run app message loop.
 * @param main_window_name Main window name.
 * @param mouse_input_queue Mouse input queue.
 * @param keyboard_input_queue Keyboard input queue.
 * @return App exit code.
 */
[[nodiscard]] int DispatchMessages(
    _In_ std::string_view main_window_name,
    wb::kernel::input::InputQueue<wb::hal::hid::MouseInput>& mouse_input_queue,
    wb::kernel::input::InputQueue<wb::hal::hid::KeyboardInput>&
        keyboard_input_queue) noexcept {
  int exit_code{0};
  bool is_done{false};
  const auto handle_quit_message = [&](const MSG& msg) noexcept {
    if (msg.message == WM_QUIT) [[unlikely]] {
      exit_code = static_cast<int>(msg.wParam);
      is_done = true;
    }
  };

  using namespace wb::ui::win;

  PeekMessageDispatcher msg_dispatcher;
  auto loop_iteration_start_time = wb::base::HighResolutionClock::now();

  // Main message app loop.
  // NOLINTNEXTLINE(bugprone-infinite-loop): Loop ends in handle_quit_message.
  while (!is_done) {
    const auto maybe_dispatch_rc =
        msg_dispatcher.Dispatch(HasNoPreDispatchMessage, handle_quit_message);

    if (maybe_dispatch_rc.has_value()) [[unlikely]] {
      const auto& rc = *maybe_dispatch_rc;

      G3PLOGE2_IF(WARNING, rc) << "Main window '" << main_window_name
                               << "' message dispatch thread received error.";

      exit_code = rc.value();
      break;
    }

    const auto now_time = wb::base::HighResolutionClock::now();
    const auto delta_time = now_time - loop_iteration_start_time;

    loop_iteration_start_time = now_time;

    wb::kernel::SimulateWorldStep(delta_time, mouse_input_queue,
                                  keyboard_input_queue);
  }

  G3LOG_IF(WARNING, exit_code != 0)
      << "Main window '" << main_window_name
      << "' message dispatch thread exited with non success code " << exit_code;

  return exit_code;
}

/**
 * @brief Makes fatal dialog context.
 * @param kernel_args Kernel arguments.
 * @return Fatal dialog context.
 */
[[nodiscard]] wb::ui::FatalDialogContext MakeFatalContext(
    const wb::kernel::KernelArgs& kernel_args) noexcept {
  return {kernel_args.intl, kernel_args.intl.Layout(), kernel_args.main_icon_id,
          kernel_args.small_icon_id};
}

}  // namespace

extern "C" [[nodiscard]] WB_WHITEBOX_KERNEL_API int KernelMain(
    const wb::kernel::KernelArgs& kernel_args) {
  using namespace wb::base;
  using namespace wb::kernel;

  const auto& intl = kernel_args.intl;
  const auto& command_line_flags = kernel_args.command_line_flags;

  using namespace wb::ui::win;

  const WindowDefinition window_definition{
      CreateMainWindowDefinition(kernel_args, kernel_args.app_description,
                                 command_line_flags.main_window_width,
                                 command_line_flags.main_window_height)};
  constexpr DWORD window_class_style{CS_HREDRAW | CS_VREDRAW};

  using namespace wb::hal::hid;

  input::InputQueue<MouseInput> mouse_input_queue;
  input::InputQueue<KeyboardInput> keyboard_input_queue;

  auto window_result =
      BaseWindow::New<MainWindow>(window_definition, window_class_style, intl,
                                  mouse_input_queue, keyboard_input_queue);
  if (MainWindow* window =
          window_result
              .transform(
                  [](const std::unique_ptr<MainWindow>& w) { return w.get(); })
              .value_or(nullptr)) [[likely]] {
    // If the window was previously visible, the return value is nonzero.  If
    // the window was previously hidden, the return value is zero.
    window->Show(kernel_args.show_window_flags);
    // Send WM_PAINT directly to draw first time.
    window->Update();

    return DispatchMessages(window_definition.name, mouse_input_queue,
                            keyboard_input_queue);
  }

  return wb::ui::FatalDialog(
      intl::l18n_fmt(intl, "{0} - Error", kernel_args.app_description),
      window_result.error(),
      intl::l18n_fmt(intl,
                     "Please, check app is installed correctly and you have "
                     "enough permissions to run it."),
      MakeFatalContext(kernel_args),
      intl::l18n_fmt(intl, "Unable to create main '{0}' window.",
                     window_definition.name));
}