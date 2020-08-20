#ifndef CPPURSES_WIDGET_WIDGETS_LABEL_HPP
#define CPPURSES_WIDGET_WIDGETS_LABEL_HPP
#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pair.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

namespace cppurses {

/// A simple Text_display with fixed height 1 and no word wrapping.
class Label : public Text_display {
   public:
    explicit Label(Glyph_string text = "") : Text_display{std::move(text)}
    {
        *this | pipe::fixed_height(1) | pipe::word_wrap(false);
    }

    /// Whether or not the width should be locked to the length of the text.
    void set_dynamic_width(bool enable)
    {
        dynamic_ = enable;
        this->set_contents(this->contents());
    }

    void set_contents(Glyph_string text)
    {
        if (dynamic_)
            *this | pipe::fixed_width(text.size());
        this->Text_display::set_contents(std::move(text));
    }

   private:
    bool dynamic_ = false;
};

/// Helper function to create an instance.
template <typename... Args>
auto label(Args&&... args) -> std::unique_ptr<Label>
{
    return std::make_unique<Label>(std::forward<Args>(args)...);
}

/// Wraps a Widget_t object with a label on the left.
template <typename Widget_t>
class Label_left : public layout::Horizontal<> {
   public:
    Label& label;
    Widget& padding = this->make_child() | pipe::fixed_width(1);
    Widget_t& wrapped;

   public:
    /// Constructs Label with \p label_, and Widget_t with args...
    template <typename... Args>
    Label_left(Glyph_string label_, Args&&... args)
        : label{this->make_child<Label>(std::move(label_))},
          wrapped{this->make_child<Widget_t>(std::forward<Args>(args)...)}
    {
        *this | pipe::fixed_height(1);
    }

   protected:
    /// Forward's Focus to wrapped.
    auto focus_in_event() -> bool override
    {
        System::set_focus(wrapped);
        return true;
    }
};

/// Helper function to create an instance.
template <typename Widget_t, typename... Args>
auto label_left(Args&&... args) -> std::unique_ptr<Label_left<Widget_t>>
{
    return std::make_unique<Label_left<Widget_t>>(std::forward<Args>(args)...);
}

/// Wraps a Widget_t object with a label on the right.
template <typename Widget_t>
class Label_right : public layout::Horizontal<> {
   public:
    Widget_t& wrapped;
    Widget& padding = this->make_child() | pipe::fixed_width(1);
    Label& label;

   public:
    /// Constructs Label with \p label_, and Widget_t with args...
    template <typename... Args>
    Label_right(Glyph_string label_, Args&&... args)
        : wrapped{this->make_child<Widget_t>(std::forward<Args>(args)...)},
          label{this->make_child<Label>(std::move(label_))}
    {
        this->height_policy = wrapped.height_policy;
    }

   protected:
    /// Forward's Focus to wrapped.
    auto focus_in_event() -> bool override
    {
        System::set_focus(wrapped);
        return true;
    }
};

/// Helper function to create an instance.
template <typename Widget_t, typename... Args>
auto label_right(Args&&... args) -> std::unique_ptr<Label_right<Widget_t>>
{
    return std::make_unique<Label_right<Widget_t>>(std::forward<Args>(args)...);
}

/// Wraps a Widget_t object with a label on the top.
template <typename Widget_t>
class Label_top : public layout::Vertical<> {
   public:
    Label& label;
    Widget& padding = this->make_child() | pipe::fixed_height(0);
    Widget_t& wrapped;

   public:
    /// Constructs Label with \p label_, and Widget_t with args...
    template <typename... Args>
    Label_top(Glyph_string label_, Args&&... args)
        : label{this->make_child<Label>(std::move(label_))},
          wrapped{this->make_child<Widget_t>(std::forward<Args>(args)...)}
    {}

   protected:
    /// Forward's Focus to wrapped.
    auto focus_in_event() -> bool override
    {
        System::set_focus(wrapped);
        return true;
    }
};

/// Helper function to create an instance.
template <typename Widget_t, typename... Args>
auto label_top(Args&&... args) -> std::unique_ptr<Label_top<Widget_t>>
{
    return std::make_unique<Label_top<Widget_t>>(std::forward<Args>(args)...);
}

/// Wraps a Widget_t object with a label on the bottom.
template <typename Widget_t>
class Label_bottom : public layout::Vertical<> {
   public:
    Widget_t& wrapped;
    Widget& padding = this->make_child() | pipe::fixed_height(0);
    Label& label;

   public:
    /// Constructs Label with \p label_, and Widget_t with args...
    template <typename... Args>
    Label_bottom(Glyph_string label_, Args&&... args)
        : wrapped{this->make_child<Widget_t>(std::forward<Args>(args)...)},
          label{this->make_child<Label>(std::move(label_))}
    {}

   protected:
    /// Forward's Focus to wrapped.
    auto focus_in_event() -> bool override
    {
        System::set_focus(wrapped);
        return true;
    }
};

/// Helper function to create an instance.
template <typename Widget_t, typename... Args>
auto label_bottom(Args&&... args) -> std::unique_ptr<Label_bottom<Widget_t>>
{
    return std::make_unique<Label_bottom<Widget_t>>(
        std::forward<Args>(args)...);
}

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_LABEL_HPP
