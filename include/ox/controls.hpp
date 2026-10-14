#pragma once

#include <cstddef>
#include <optional>

#include <ox/core/events.hpp>

namespace ox {

/** Unmodified Enter/Space presses, or the left mouse button. */
[[nodiscard]] auto activates(KeyEvent const& event) -> bool;
[[nodiscard]] auto activates(MousePress const& event) -> bool;
/** Release matching an activation key, or left/unspecified mouse button. */
[[nodiscard]] auto releases(KeyEvent const& event) -> bool;
[[nodiscard]] auto releases(MouseRelease const& event) -> bool;

/** Change an optional index without owning items, signals, or repaint scheduling. */
[[nodiscard]] auto select_index(std::optional<std::size_t>& selected,
                                std::size_t index,
                                std::size_t count) -> EventResponse;
[[nodiscard]] auto move_selection(std::optional<std::size_t>& selected,
                                  std::size_t count,
                                  int steps) -> EventResponse;

}  // namespace ox
