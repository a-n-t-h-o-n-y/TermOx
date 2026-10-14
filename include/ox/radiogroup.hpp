#pragma once

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include <signals_light/signal.hpp>

#include <ox/widget.hpp>

namespace ox {

/**
 * A vertical grouping of labeled radio buttons, with exclusive selection via mouse and
 * keyboard.
 *
 * @details Each entry is a single horizontal line. If Focus is enabled, arrow keys and
 * enter are used to make selections.
 *
 *  .on = U" ⬤"_gs,
 *  .off = U" ◯"_gs,
 */
class RadioGroup : public Widget {
   public:
    using Widget::handle;

    struct Options {
        std::vector<std::string> items = {};
        std::optional<std::size_t> selected_index = std::nullopt;
        Style style = {};
        Style in_focus_style = {.traits = Trait::Bold};
        std::vector<Glyph> on = U"(X)"_gs;
        std::vector<Glyph> off = U"( )"_gs;
        FocusPolicy focus_policy = FocusPolicy::None;
        SizePolicy size_policy = SizePolicy::flex();
    } static const init;

    std::vector<std::string> items;
    std::optional<std::size_t> selected_index;
    Style style;
    Style in_focus_style;
    std::vector<Glyph> on;
    std::vector<Glyph> off;

    sl::Signal<void(std::string const&)> on_select;

   public:
    RadioGroup(Options x = init);

   public:
    auto paint(Canvas c) -> Cursor override;

    auto handle(KeyEvent const& k) -> EventResponse override;

    auto handle(MousePress const& m) -> EventResponse override;

    auto focus_in() -> EventResponse override;

    auto focus_out() -> EventResponse override;

   private:
    std::optional<std::size_t> focus_selection_ = std::nullopt;
    bool in_focus_ = false;
};

}  // namespace ox