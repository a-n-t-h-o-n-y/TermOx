#ifndef TERMOX_WIDGET_WIDGETS_LINE_HPP
#define TERMOX_WIDGET_WIDGETS_LINE_HPP
#include <memory>

#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// A Vertical or Horizontal single width line divider.
template <template <typename> typename Layout_t>
class Line : public Widget {
   public:
    struct Parameters {};

   public:
    Line();

    explicit Line(Parameters);

   private:
    static auto constexpr is_vertical = layout::is_vertical_v<Layout_t<Widget>>;
};

/// Helper function to create a Line instance.
template <template <typename> typename Layout_t>
[[nodiscard]] auto line() -> std::unique_ptr<Line<Layout_t>>;

/// Helper function to create a Line instance.
template <template <typename> typename Layout_t>
[[nodiscard]] auto line(typename Line<Layout_t>::Parameters p)
    -> std::unique_ptr<Line<Layout_t>>;

using VLine = Line<layout::Vertical>;

/// Helper function to create a VLine instance.
[[nodiscard]] auto vline() -> std::unique_ptr<VLine>;

/// Helper function to create a VLine instance.
[[nodiscard]] auto vline(VLine::Parameters p) -> std::unique_ptr<VLine>;

using HLine = Line<layout::Horizontal>;

/// Helper function to create an HLine instance.
[[nodiscard]] auto hline() -> std::unique_ptr<HLine>;

/// Helper function to create an HLine instance.
[[nodiscard]] auto hline(typename HLine::Parameters p)
    -> std::unique_ptr<HLine>;

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_LINE_HPP
