#ifndef TERMOX_WIDGET_WIDGETS_TITLEBAR_HPP
#define TERMOX_WIDGET_WIDGETS_TITLEBAR_HPP
#include <memory>

#include <termox/painter/glyph_string.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/label.hpp>

namespace ox {

/// Provides a centered title and flush-right exit button.
/** The exit button will call System::quit() when pressed. Titlebar is fixed at
 *  a height of 1. */
// class Titlebar : public layout::Horizontal<> {
class Titlebar : public HPair<HLabel, Button> {
   public:
    struct Parameters {
        Glyph_string title_text = U"";
    };

   public:
    HLabel& title       = this->first;
    Button& exit_button = this->second;

   public:
    /// Construct a Titlebar with centered \p title.
    explicit Titlebar(Glyph_string title_text = U"");

    /// Construct a Titlebar from \p p.
    explicit Titlebar(Parameters p);

   private:
    inline static auto constexpr exit_width  = 3;
    inline static auto constexpr extra_left  = 0;
    inline static auto constexpr extra_right = exit_width;
};

/// Helper function to create a Titlebar instance.
[[nodiscard]] auto titlebar(Glyph_string title_text = U"")
    -> std::unique_ptr<Titlebar>;

/// Helper function to create a Titlebar instance.
[[nodiscard]] auto titlebar(Titlebar::Parameters p)
    -> std::unique_ptr<Titlebar>;

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_TITLEBAR_HPP
