#ifndef TERMOX_WIDGET_WIDGETS_SCROLLBAR_HPP
#define TERMOX_WIDGET_WIDGETS_SCROLLBAR_HPP
#include <cstddef>
#include <memory>

#include <signals_light/signal.hpp>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/widget/layouts/detail/linear_layout.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/button.hpp>
#include <termox/widget/widgets/textbox.hpp>

namespace ox {

/// Scrollbar to display progress through some structure, and control progress.
/** Provides an increment, decrement, and new_position Signals to notify of
 *  changes. The size parameter tells the scrollbar how many units of scroll
 *  there are to the scructure it is linked with, and the current position the
 *  scrollbar is at is emitted with the new_position signal. */
template <template <typename> typename Layout_t>
class Scrollbar : public Layout_t<Widget> {
   public:
    struct Parameters {
        std::size_t size     = 0;
        std::size_t position = invalid_position;
    };

   private:
    class Middle : public Widget {
       public:
        using Parameters = Scrollbar::Parameters;

       public:
        static auto constexpr invalid_position = -1uL;

       public:
        explicit Middle(std::size_t size = 0, std::size_t position = 0);

        explicit Middle(Parameters p);

       public:
        void set_position(std::size_t p);

        void set_size(std::size_t s);

        void set_bar_fg(Color c);

        void set_bar_bg(Color c);

        /// Finds the scroll position from a physical point on the bar.
        [[nodiscard]] auto find_position_from_point(Point p) -> std::size_t;

       protected:
        auto paint_event(Painter& p) -> bool override;

        auto resize_event(Area new_size, Area old_size) -> bool override;

       private:
        static auto constexpr is_vertical =
            layout::is_vertical_v<Layout_t<Widget>>;

       private:
        std::size_t size_;
        std::size_t position_;

        Glyph bar_ = is_vertical ? U'█' : U'▬';

        int slider_position_;
        std::size_t slider_length_;

       private:
        void update_length_and_position();

        [[nodiscard]] auto max_length() const -> std::size_t;
    };

   public:
    static auto constexpr invalid_position = -1uL;

   public:
    Button& decrement_btn =
        this->template make_child<Button>(Glyph_string{top_symbol_});

    Middle& middle = this->template make_child<Middle>({0, invalid_position});

    Button& increment_btn =
        this->template make_child<Button>(Glyph_string{bottom_symbol_});

   public:
    sl::Signal<void(std::size_t)> new_position;
    sl::Signal<void()> decremented;
    sl::Signal<void()> incremented;

   public:
    explicit Scrollbar(std::size_t size     = 0,
                       std::size_t position = invalid_position);

    explicit Scrollbar(Parameters p);

   public:
    [[nodiscard]] auto get_size() const -> std::size_t;

    void set_size(std::size_t s);

    void increment_size();

    void decrement_size();

    [[nodiscard]] auto get_position() const -> std::size_t;

    void set_position(std::size_t p);

    void decrement_position();

    void increment_position();

    /// Returns true if button was a scoll wheel button
    auto handle_wheel(Mouse::Button button) -> bool;

   protected:
    auto mouse_wheel_event_filter(Widget&, Mouse const& m) -> bool override;

    auto mouse_press_event_filter(Widget& w, Mouse const& m) -> bool override;

    auto mouse_move_event_filter(Widget& w, Mouse const& m) -> bool override;

   private:
    std::size_t size_;
    std::size_t position_;

   private:
    static auto constexpr is_vertical = layout::is_vertical_v<Layout_t<Widget>>;
    static auto constexpr top_symbol_ = is_vertical ? U'▴' : U'◂';
    static auto constexpr bottom_symbol_ = is_vertical ? U'▾' : U'▸';
};

/// Helper function to create a Scrollbar instance.
template <template <typename> typename Layout_t>
[[nodiscard]] auto scrollbar(
    std::size_t size     = 0,
    std::size_t position = Scrollbar<Layout_t>::invalid_position)
    -> std::unique_ptr<Scrollbar<Layout_t>>;

/// Helper function to create a Scrollbar instance.
template <template <typename> typename Layout_t>
[[nodiscard]] auto scrollbar(typename Scrollbar<Layout_t>::Parameters p)
    -> std::unique_ptr<Scrollbar<Layout_t>>;

using HScrollbar = Scrollbar<layout::Horizontal>;

/// Helper function to create an HScrollbar instance.
[[nodiscard]] auto hscrollbar(
    std::size_t size     = 0,
    std::size_t position = HScrollbar::invalid_position)
    -> std::unique_ptr<HScrollbar>;

/// Helper function to create an HScrollbar instance.
[[nodiscard]] auto hscrollbar(HScrollbar::Parameters p)
    -> std::unique_ptr<HScrollbar>;

using VScrollbar = Scrollbar<layout::Vertical>;

/// Helper function to create a VScrollbar instance.
[[nodiscard]] auto vscrollbar(
    std::size_t size     = 0,
    std::size_t position = VScrollbar::invalid_position)
    -> std::unique_ptr<VScrollbar>;

/// Helper function to create a VScrollbar instance.
[[nodiscard]] auto vscrollbar(VScrollbar::Parameters p)
    -> std::unique_ptr<VScrollbar>;

/// Link a scrollbar and any Linear_layout type.
template <template <typename> typename Layout_t,
          typename Child,
          typename Parameters>
void link(Scrollbar<Layout_t>& scrollbar,
          layout::detail::Linear_layout<Child, Parameters>& layout,
          bool hijack_scroll = true)
{
    scrollbar.set_size(layout.child_count());

    scrollbar.new_position.connect([&](std::size_t p) {
        if (p < layout.child_count())
            layout.set_child_offset(p);
    });

    layout.child_added.connect([&](auto&) { scrollbar.increment_size(); });
    layout.child_removed.connect([&](auto&) { scrollbar.decrement_size(); });
    if (hijack_scroll) {
        layout.child_added.connect([&](auto& child) {
            child.install_event_filter(scrollbar);
            for (Widget* descendant : child.get_descendants())
                descendant->install_event_filter(scrollbar);
        });
        scrollbar.mouse_wheel_scrolled_filter.connect(
            [&](auto&, auto const& mouse) {
                scrollbar.handle_wheel(mouse.button);
            });
        layout.mouse_wheel_scrolled.connect(
            [&](auto const& mouse) { scrollbar.handle_wheel(mouse.button); });
    }
}

/// Link a Scrollbar and Textbox together.
template <template <typename> typename Layout_t>
void link(Scrollbar<Layout_t>& scrollbar, Textbox& textbox);

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_SCROLLBAR_HPP
