#ifndef CPPURSES_WIDGET_WIDGETS_LABEL_HPP
#define CPPURSES_WIDGET_WIDGETS_LABEL_HPP
#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/pair.hpp>
#include <cppurses/widget/widgets/fixed_width.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

namespace cppurses {

/// A simple Text_display with fixed height 1 and no word wrapping.
class Label : public Text_display {
   public:
    explicit Label(Glyph_string text = "") : Text_display{std::move(text)}
    {
        this->height_policy.fixed(1);
        this->disable_word_wrap();
    }
};

/// Wraps a Widget_t object with a label on the left.
template <typename Widget_t>
class Label_left : public layout::Horizontal<> {
   public:
    Label& label;
    Fixed_width& padding = this->make_child<Fixed_width>(1);
    Widget_t& wrapped;

   public:
    /// Constructs Label with \p label_, and Widget_t with args...
    template <typename... Args>
    Label_left(Glyph_string label_, Args&&... args)
        : label{this->make_child<Label>(std::move(label_))},
          wrapped{this->make_child<Widget_t>(std::forward<Args>(args)...)}
    {
        this->height_policy.fixed(1);
    }
};

/// Wraps a Widget_t object with a label on the right.
template <typename Widget_t>
class Label_right : public layout::Horizontal<> {
   public:
    Widget_t& wrapped;
    Fixed_width& padding = this->make_child<Fixed_width>(1);
    Label& label;

   public:
    /// Constructs Label with \p label_, and Widget_t with args...
    template <typename... Args>
    Label_right(Glyph_string label_, Args&&... args)
        : wrapped{this->make_child<Widget_t>(std::forward<Args>(args)...)},
          label{this->make_child<Label>(std::move(label_))}
    {
        this->height_policy.fixed(1);
    }
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_LABEL_HPP
