#ifndef TERMOX_WIDGET_WIDGETS_COLOR_SELECT_HPP
#define TERMOX_WIDGET_WIDGETS_COLOR_SELECT_HPP
#include <memory>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
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
    explicit Color_tile(Color c, Display display = Display::None);

    explicit Color_tile(Parameters p);
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
        Color_tile::Display display = Color_tile::Display::None);

    explicit Color_select(Parameters p);

   private:
    void set_palette(Palette const& pal);

   private:
    Color_tile::Display display_;
};

/// Helper function to create a Color_select instance.
[[nodiscard]] auto color_select(
    Color_tile::Display display = Color_tile::Display::None)
    -> std::unique_ptr<Color_select>;

/// Helper function to create a Color_select instance.
[[nodiscard]] auto color_select(Color_select::Parameters p)
    -> std::unique_ptr<Color_select>;

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_COLOR_SELECT_HPP
