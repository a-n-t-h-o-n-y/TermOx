#ifndef TERMOX_WIDGET_WIDGETS_COLOR_SELECT_HPP
#define TERMOX_WIDGET_WIDGETS_COLOR_SELECT_HPP
#include <memory>
#include <string>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/painter/painter.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/button.hpp>

namespace ox {

class Color_tile : public Button {
   public:
    enum class Display { None, Number };

    struct Parameters {
        Color color;
        Display display = Display::None;
    };

   public:
    explicit Color_tile(Color c, Display display = Display::None)
        : Button{display == Display::None ? "" : std::to_string(c.value)}
    {
        *this | bg(c);
    }

    explicit Color_tile(Parameters parameters)
        : Color_tile{parameters.color, parameters.display}
    {}
};

using Color_line = layout::Horizontal<Color_tile>;

/// Displays each color of the current palette.
/** Updates when Terminal::set_palette() succeeds. */
class Color_select : public layout::Vertical<Color_line> {
   public:
    struct Parameters {
        Color_tile::Display display = Color_tile::Display::None;
    };

   public:
    sl::Signal<void(Color)> color_selected;

   public:
    explicit Color_select(
        Color_tile::Display display = Color_tile::Display::None)
        : display_{display}
    {
        this->set_palette(Terminal::current_palette());
        Terminal::palette_changed.connect(
            [this](auto const& pal) { this->set_palette(pal); });
    }

    explicit Color_select(Parameters parameters)
        : Color_select{parameters.display}
    {}

   private:
    void set_palette(Palette const& pal)
    {
        this->delete_all_children();
        auto const size           = pal.size();
        auto constexpr row_length = 8uL;
        auto count                = 0uL;

        while (count != size) {
            auto& color_line = this->make_child();
            for (auto i = 0uL; i < row_length && count != size; ++i, ++count) {
                auto const color = pal[count].color;
                color_line.make_child(color, display_) |
                    pipe::on_press(
                        [this, color] { color_selected.emit(color); });
            }
        }
    }

   private:
    Color_tile::Display display_;
};

/// Helper function to create a Color_select instance.
[[nodiscard]] inline auto color_select(
    Color_tile::Display display = Color_tile::Display::None)
    -> std::unique_ptr<Color_select>
{
    return std::make_unique<Color_select>(display);
}

/// Helper function to create a Color_select instance.
[[nodiscard]] inline auto color_select(Color_select::Parameters parameters)
    -> std::unique_ptr<Color_select>
{
    return std::make_unique<Color_select>(std::move(parameters));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_COLOR_SELECT_HPP
