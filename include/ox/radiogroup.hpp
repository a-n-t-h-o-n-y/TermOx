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
    struct Options {
        std::vector<std::string> items = {};
        std::optional<std::size_t> selected_index = std::nullopt;
        Brush brush = {};
        Brush in_focus_brush = {.traits = Trait::Bold};
        std::vector<Glyph> on = U"(X)"_gs;
        std::vector<Glyph> off = U"( )"_gs;
        FocusPolicy focus_policy = FocusPolicy::None;
        SizePolicy size_policy = SizePolicy::flex();
    } static const init;

    std::vector<std::string> items;
    std::optional<std::size_t> selected_index;
    Brush brush;
    Brush in_focus_brush;
    std::vector<Glyph> on;
    std::vector<Glyph> off;

    sl::Signal<void(std::string const&)> on_select;

   public:
    RadioGroup(Options x = init);

   public:
    void paint(Canvas c) override;

    void key_press(Key k) override;

    void mouse_press(Mouse m) override;

    void focus_in() override;

    void focus_out() override;

   private:
    std::optional<std::size_t> focus_selection_ = std::nullopt;
    bool in_focus_ = false;
};

}  // namespace ox