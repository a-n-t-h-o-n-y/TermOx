#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <string>
#include <vector>

#include <signals_light/signal.hpp>

#include <ox/core/text.hpp>
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
        StylePatch focused_style = {.background = {},
                                    .foreground = {},
                                    .enabled = Trait::Bold,
                                    .disabled = {}};
        std::vector<Glyph> on = U"(X)"_gs;
        std::vector<Glyph> off = U"( )"_gs;
        FocusPolicy focus_policy = FocusPolicy::Strong;
        SizePolicy width_policy = SizePolicy::flex();
        SizePolicy height_policy = SizePolicy::flex();
    } static const init;

    auto items() const -> std::span<std::string const> { return items_; }
    auto selected_index() const -> std::optional<std::size_t> { return selected_; }
    void set_items(std::vector<std::string> items);
    Style style;
    StylePatch focused_style;
    std::vector<Glyph> on;
    std::vector<Glyph> off;

    auto highlighted_index() const -> std::optional<std::size_t>
    { return highlighted_; }
    sl::Signal<void(std::optional<std::size_t>)> on_select;

   public:
    RadioGroup(Options x = init);
    RadioGroup(RadioGroup&& other);
    auto operator=(RadioGroup&& other) -> RadioGroup&;

   public:
    auto select(std::optional<std::size_t> index) -> bool;

    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas c) -> Cursor override;

    auto handle(KeyEvent const& k) -> EventResponse override;

    auto handle(MousePress const& m) -> EventResponse override;

    auto focus_in() -> EventResponse override;

    auto focus_out() -> EventResponse override;

   private:
    std::vector<std::string> items_;
    std::optional<std::size_t> selected_;
    std::optional<std::size_t> highlighted_;
    mutable std::vector<TextLayout> measure_layouts_;
};

}  // namespace ox