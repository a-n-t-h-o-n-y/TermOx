/** @file
 * Activation-key recognition and index-selection helpers.
 */
#pragma once

#include <cstddef>
#include <optional>

#include <ox/core/events.hpp>

namespace ox {

/** Unmodified Enter/Space presses, or the left mouse button. */
[[nodiscard]] auto activates(KeyEvent const& event) -> bool;
/** Return true for a left mouse press, independently of widget geometry or enabled state. */
[[nodiscard]] auto activates(MousePress const& event) -> bool;
/** Release matching an activation key, or left/unspecified mouse button. */
[[nodiscard]] auto releases(KeyEvent const& event) -> bool;
/** Return true for a left or unspecified mouse-button release. */
[[nodiscard]] auto releases(MouseRelease const& event) -> bool;

/** Change an optional index without owning items, signals, or repaint scheduling. */
[[nodiscard]] auto select_index(std::optional<std::size_t>& selected,
                                std::size_t index,
                                std::size_t count) -> EventResponse;
/** Move a borrowed optional index by clamped steps and return handled/repaint effects;
 * an empty model remains unselected.
 */
[[nodiscard]] auto move_selection(std::optional<std::size_t>& selected,
                                  std::size_t count,
                                  int steps) -> EventResponse;

}  // namespace ox
