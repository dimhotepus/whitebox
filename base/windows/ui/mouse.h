// Copyright (c) 2021 The WhiteBox Authors.  All rights reserved.
// Use of this source code is governed by a 3-Clause BSD license that can be
// found in the LICENSE file.
//
// Mouse input device.

#ifndef WB_BASE_WINDOWS_UI_MOUSE_H_
#define WB_BASE_WINDOWS_UI_MOUSE_H_

#include <string>
#include <system_error>

#include "base/base_api.h"
#include "base/base_macroses.h"

using HWND = struct HWND__ *;
using RAWINPUT = struct tagRAWINPUT;

namespace wb::base::windows::ui {
/**
 * @brief The mouse state.  Can be any reasonable combination of the following.
 */
enum class MouseStateFlags : unsigned short {
  /**
   * @brief Mouse movement data is relative to the last mouse position.  Note
   * default Windows code is 0, but we remap to this as it is hard to check via
   * bitwise & when ex. any other flag also used.
   */
  kMoveRelative = 0x8000U,
  /**
   * @brief Mouse movement data is based on absolute position.
   */
  kMoveAbsolute = 0x0001U,
  /**
   * @brief Mouse coordinates are mapped to the virtual desktop (for a multiple
   * monitor system).
   */
  kVirtualDesktop = 0x0002U,
  /**
   * @brief Mouse attributes changed; application needs to query the mouse
   * attributes.
   */
  kMouseAttributesChanged = 0x0004U,
  /**
   * @brief This mouse movement event was not coalesced.  Mouse movement events
   * can be coalescened by default.
   */
  kMouseMoveNoCoalesce = 0x0008U
};

/**
 * @brief operator&.
 * @param left Left.
 * @param right Right
 * @return left & right.
 */
[[nodiscard]] constexpr MouseStateFlags operator&(
    MouseStateFlags left, MouseStateFlags right) noexcept {
  return static_cast<MouseStateFlags>(underlying_cast(left) &
                                      underlying_cast(right));
}

/**
 * @brief operator|.
 * @param left Left.
 * @param right Right
 * @return left | right.
 */
[[nodiscard]] constexpr MouseStateFlags operator|(
    MouseStateFlags left, MouseStateFlags right) noexcept {
  return static_cast<MouseStateFlags>(underlying_cast(left) |
                                      underlying_cast(right));
}

/**
 * @brief Mouse state as string.
 * @param state_flags Mouse state.
 * @return string.
 */
[[nodiscard]] inline std::string to_string(MouseStateFlags state_flags) {
  std::string result;
  result.reserve(64);

  if ((state_flags & MouseStateFlags::kMoveRelative) ==
      MouseStateFlags::kMoveRelative) {
    result += "Relative";
  } else if ((state_flags & MouseStateFlags::kMoveAbsolute) ==
             MouseStateFlags::kMoveAbsolute) {
    result += "Absolute";
  }

  if ((state_flags & MouseStateFlags::kVirtualDesktop) ==
      MouseStateFlags::kVirtualDesktop) {
    result += "| Virtual Desktop";
  }

  if ((state_flags & MouseStateFlags::kMouseAttributesChanged) ==
      MouseStateFlags::kMouseAttributesChanged) {
    result += "| Attributes Changed";
  }

  if ((state_flags & MouseStateFlags::kMouseMoveNoCoalesce) ==
      MouseStateFlags::kMouseMoveNoCoalesce) {
    result += "| Move No Coalesce";
  }

  return result;
};

/**
 * @brief The transition state of the mouse buttons.  Can be one or more of the
 * following values.
 */
enum class MouseButtonTransitionState : unsigned short {
  /**
   * @brief No transition.
   */
  kNone = 0x0000U,
  /**
   * @brief Left button changed to down.
   */
  kLeftButtonDown = 0x0001U,
  /**
   * @brief Left button changed to up.
   */
  kLeftButtonUp = 0x0002U,
  /**
   * @brief Right button changed to down.
   */
  kRightButtonDown = 0x0004U,
  /**
   * @brief Right button changed to up.
   */
  kRightButtonUp = 0x0008U,
  /**
   * @brief Middle button changed to down.
   */
  kMiddleButtonDown = 0x0010U,
  /**
   * @brief Middle button changed to up.
   */
  kMiddleButtonUp = 0x0020U,

  /**
   * @brief XBUTTON1 changed to down.
   */
  kXButton1Down = 0x0040U,
  /**
   * @brief XBUTTON1 changed to up.
   */
  kXButton1Up = 0x0080U,
  /**
   * @brief XBUTTON2 changed to down.
   */
  kXButton2Down = 0x0100U,
  /**
   * @brief XBUTTON2 changed to up.
   */
  kXButton2Up = 0x0200U,

  /**
   * @brief Raw input comes from a mouse wheel.  The wheel delta is stored in
   * MouseInput.button_data.  A positive value indicates that the wheel was
   * rotated forward, away from the user; a negative value indicates that the
   * wheel was rotated backward, toward the user.
   */
  kVerticalWheel = 0x0400U,
  /**
   * @brief Raw input comes from a horizontal mouse wheel.  The wheel delta is
   * stored in usButtonData.  A positive value indicates that the wheel was
   * rotated to the right; a negative value indicates that the wheel was rotated
   * to the left.
   */
  kHorizontalWheel = 0x0800U
};

/**
 * @brief operator&.
 * @param left Left.
 * @param right Right
 * @return left & right.
 */
[[nodiscard]] constexpr MouseButtonTransitionState operator&(
    MouseButtonTransitionState left,
    MouseButtonTransitionState right) noexcept {
  return static_cast<MouseButtonTransitionState>(underlying_cast(left) &
                                                 underlying_cast(right));
}

/**
 * @brief Mouse button transition state as string.
 * @param state_flags Mouse button transition state.
 * @return string.
 */
[[nodiscard]] inline std::string to_string(
    MouseButtonTransitionState state_flags) {
  std::string result;
  result.reserve(128);

  if ((state_flags & MouseButtonTransitionState::kLeftButtonDown) ==
      MouseButtonTransitionState::kLeftButtonDown) {
    result += "Left Down";
  } else if ((state_flags & MouseButtonTransitionState::kLeftButtonUp) ==
             MouseButtonTransitionState::kLeftButtonUp) {
    result += "Left Up";
  }

  if ((state_flags & MouseButtonTransitionState::kRightButtonDown) ==
      MouseButtonTransitionState::kRightButtonDown) {
    result += "Right Down ";
  } else if ((state_flags & MouseButtonTransitionState::kRightButtonUp) ==
             MouseButtonTransitionState::kRightButtonUp) {
    result += "Right Up ";
  }

  if ((state_flags & MouseButtonTransitionState::kMiddleButtonDown) ==
      MouseButtonTransitionState::kMiddleButtonDown) {
    result += "Middle Down ";
  } else if ((state_flags & MouseButtonTransitionState::kMiddleButtonUp) ==
             MouseButtonTransitionState::kMiddleButtonUp) {
    result += "Middle Up ";
  }

  if ((state_flags & MouseButtonTransitionState::kXButton1Down) ==
      MouseButtonTransitionState::kXButton1Down) {
    result += "X1 Down ";
  } else if ((state_flags & MouseButtonTransitionState::kXButton1Up) ==
             MouseButtonTransitionState::kXButton1Up) {
    result += "X1 Up ";
  }

  if ((state_flags & MouseButtonTransitionState::kXButton2Down) ==
      MouseButtonTransitionState::kXButton2Down) {
    result += "X2 Down ";
  } else if ((state_flags & MouseButtonTransitionState::kXButton2Up) ==
             MouseButtonTransitionState::kXButton2Up) {
    result += "X2 Up ";
  }

  if ((state_flags & MouseButtonTransitionState::kVerticalWheel) ==
      MouseButtonTransitionState::kVerticalWheel) {
    result += "Vertical Wheel ";
  }

  if ((state_flags & MouseButtonTransitionState::kHorizontalWheel) ==
      MouseButtonTransitionState::kHorizontalWheel) {
    result += "Horizontal Wheel ";
  }

  return result.size() ? result : "None";
};

/**
 * @brief Mouse input.
 */
struct MouseInput {
  /**
   * @brief Mouse state flags.
   *
   * If the mouse has moved, indicated by MouseStateFlags::kMoveRelative or
   * MouseStateFlags::kMoveAbsolute, last_x and last_y specify information about
   * that movement.  The information is specified as relative or absolute
   * integer values.
   *
   * If MouseStateFlags::kMoveRelative value is specified, last_x and last_y
   * specify movement relative to the previous mouse event (the last reported
   * position).  Positive values mean the mouse moved right (or down); negative
   * values mean the mouse moved left (or up).
   *
   * If MouseStateFlags::kMoveAbsolute value is specified, last_x and last_y
   * contain normalized absolute coordinates between 0 and 65,535.  Coordinate
   * (0,0) maps onto the upper-left corner of the display surface; coordinate
   * (65535,65535) maps onto the lower-right corner.  In a multimonitor system,
   * the coordinates map to the primary monitor.
   *
   * If MouseStateFlags::kVirtualDesktop is specified in addition to
   * MouseStateFlags::kMoveAbsolute, the coordinates map to the entire virtual
   * desktop.
   *
   * In contrast to legacy WM_MOUSEMOVE window messages Raw Input mouse events
   * is not subject to the effects of the mouse speed set in the Control Panel's
   * Mouse Properties sheet.
   */
  MouseStateFlags mouse_state;

  /**
   * The transition state of the mouse buttons.
   */
  MouseButtonTransitionState button_flags;

  /**
   * @brief If mouse wheel is moved, indicated by
   * MouseButtonTransitionState::kVerticalWheel or
   * MouseButtonTransitionState::kHorizontalWheel in button_flags, then
   * button_data contains a float value that specifies the distance the
   * wheel is rotated.
   */
  float button_data;

  /*
   * The signed relative or absolute motion in the X direction.
   */
  long last_x;

  /*
   * The signed relative or absolute motion in the Y direction.
   */
  long last_y;
};

/**
 * @brief Mouse input as string.
 * @param mouse_input Mouse input.
 * @return string.
 */
[[nodiscard]] inline std::string to_string(const MouseInput &mouse_input) {
  std::string result;
  result.reserve(256);

  using std::to_string;

  result += "State: " + to_string(mouse_input.mouse_state);
  result += " | Buttons: " + to_string(mouse_input.button_flags);
  result += ((mouse_input.button_flags &
              MouseButtonTransitionState::kVerticalWheel) ==
                 MouseButtonTransitionState::kVerticalWheel ||
             (mouse_input.button_flags &
              MouseButtonTransitionState::kHorizontalWheel) ==
                 MouseButtonTransitionState::kHorizontalWheel)
                ? (" | Wheel Data: " + to_string(mouse_input.button_data))
                : "";
  result += " | Last X: " + to_string(mouse_input.last_x);
  result += " | Last Y: " + to_string(mouse_input.last_y);

  return result;
}

/**
 * @brief Low level mouse input device.
 */
class Mouse {
 public:
  /**
   * @brief Creates mouse device.
   * @param window Window to handle mouse input.
   * @return nothing.
   */
  WB_BASE_API Mouse(_In_ HWND window) noexcept;
  /**
   * @brief Shut down mouse device.
   */
  WB_BASE_API ~Mouse() noexcept;

  Mouse(Mouse &&) = default;
  Mouse &operator=(Mouse &&) = default;

  WB_NO_COPY_CTOR_AND_ASSIGNMENT(Mouse);

  /**
   * @brief Mouse initialization error code.
   * @return Error code.
   */
  [[nodiscard]] std::error_code error_code() const noexcept {
    return error_code_;
  }

  /**
   * @brief Handle raw input.
   * @param raw_input Raw input.
   * @param mouse_input Raw input as mouse input if it is mouse input.
   * @return true if raw input is mouse input, false otherwise.
   */
  [[nodiscard]] WB_BASE_API bool Handle(const RAWINPUT &raw_input,
                                        MouseInput &mouse_input) noexcept;

 private:
  /**
   * @brief Window handle to get mouse input for.
   */
  HWND window_;
  /**
   * @brief Mouse initialization error code.
   */
  std::error_code error_code_;
};
}  // namespace wb::base::windows::ui

#endif  // !WB_BASE_WINDOWS_UI_MOUSE_H_