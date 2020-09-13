#ifndef CPPURSES_WIDGET_WIDGETS_LABEL_HPP
#define CPPURSES_WIDGET_WIDGETS_LABEL_HPP
#include <cstddef>
#include <memory>
#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/align.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>

namespace cppurses {

/// A single line of text with alignment, non-editable.
/** Can be either Vertical or Horizontal by passing in a Layout type. */
template <typename Layout_t>
class Label : public Widget {
    static_assert(layout::is_vertical_v<Layout_t> ||
                  layout::is_horizontal_v<Layout_t>);

   public:
    struct Parameters {
        Glyph_string text;
        Align alignment         = Align::Left;
        std::size_t extra_left  = 0uL;    // Accounts for extra space on left or
        std::size_t extra_right = 0uL;    // right side of Widget for centering.
        bool dynamic_size       = false;  // Resize Widget to text.size().
    };

   public:
    /// Create a new Label with given parameters.
    explicit Label(Parameters p) : params_{std::move(p)}
    {
        if constexpr (is_vertical)
            *this | pipe::fixed_width(1uL);
        else
            *this | pipe::fixed_height(1uL);
    }

   public:
    /// Set text contents of Label and update display.
    void set_text(Glyph_string text)
    {
        if (params_.dynamic_size)
            *this | pipe::fixed_width(text.size());
        params_.text = std::move(text);
        this->update_offset();
    }

    /// Return the text given to set_text().
    auto get_text() const -> Glyph_string const& { return params_.text; }

    /// Set text alignment of Label and update display.
    void set_alignment(Align x)
    {
        params_.alignment = x;
        this->update_offset();
    }

    /// Return the Align given to set_alignment().
    auto get_alignment() const -> Align { return params_.alignment; }

    /// Inform Label about space to left of Label for centered text offset.
    void set_extra_left(std::size_t x)
    {
        params_.extra_left = x;
        this->update_offset();
    }

    /// Return the amount given to set_extra_left().
    auto get_extra_left() const -> std::size_t { return params_.extra_left; }

    /// Inform Label about space to right of Label for centered text offset.
    void set_extra_right(std::size_t x)
    {
        params_.extra_right = x;
        this->update_offset();
    }

    /// Return the amount given to set_extra_right().
    auto get_extra_right() const -> std::size_t { return params_.extra_right; }

    /// Enable/Disable dynamic size, where the Label's size is the text length.
    void set_dynamic_size(bool enable)
    {
        params_.dynamic_size = enable;
        if (params_.dynamic_size) {
            if constexpr (is_vertical)
                *this | pipe::fixed_height(params_.text.size());
            else
                *this | pipe::fixed_width(params_.text.size());
        }
    }

    /// Return the value given to set_dynamic_size().
    auto get_dynamic_size() const -> bool { return params_.dynamic_size; }

   protected:
    auto paint_event() -> bool override
    {
        if constexpr (is_vertical)
            this->paint_vertical();
        else
            this->paint_horizontal();
        return Widget::paint_event();
    }

    auto resize_event(Area new_size, Area old_size) -> bool override
    {
        this->update_offset();
        return Widget::resize_event(new_size, old_size);
    }

   private:
    inline static auto constexpr is_vertical = layout::is_vertical_v<Layout_t>;
    Parameters params_;
    std::size_t offset_ = 0uL;

   private:
    /// Update the internal offset_ value to account for new settings/state
    void update_offset()
    {
        auto const box_length = [this] {
            if constexpr (is_vertical)
                return this->height();
            else
                return this->width();
        }();

        offset_ = this->find_offset(params_.text.size(), box_length,
                                    params_.extra_left, params_.extra_right);
        this->update();
    }

    void paint_vertical()
    {
        auto p = Painter{*this};
        for (auto i = 0uL; i < params_.text.size() && i < this->height(); ++i)
            p.put(params_.text[i], {0uL, offset_ + i});
    }

    void paint_horizontal()
    {
        Painter{*this}.put(params_.text, {offset_, 0uL});
    }

    /// Find the text's first Glyph placement along the length of the Widget.
    auto find_offset(std::size_t text_length,
                     std::size_t box_length,
                     std::size_t extra_left,
                     std::size_t extra_right) -> std::size_t
    {
        switch (params_.alignment) {
            case Align::Left:
            case Align::Top: return left_top_offset();

            case Align::Center:
                return apply_origin_offset(
                    center_offset(text_length,
                                  box_length + extra_left + extra_right),
                    extra_left);

            case Align::Right:
            case Align::Bottom:
                return right_bottom_offset(text_length, box_length);
        }
        return 0uL;
    }

    static auto left_top_offset() -> std::size_t { return 0uL; }

    static auto center_offset(std::size_t text_length, std::size_t box_length)
        -> std::size_t
    {
        return text_length > box_length ? 0uL : (box_length - text_length) / 2;
    }

    static auto right_bottom_offset(std::size_t text_length,
                                    std::size_t box_length) -> std::size_t
    {
        return text_length > box_length ? 0uL : box_length - text_length;
    }

    /// Offsets a given \p position by the extra length parameters
    static auto apply_origin_offset(std::size_t position, std::size_t left)
        -> std::size_t
    {
        position -= position < left ? position : left;
        return position;
    }
};

/// Horizontal Label Widget
using HLabel = Label<layout::Horizontal<>>;

/// Vertical Label Widget
using VLabel = Label<layout::Vertical<>>;

/// Helper function to create an instance.
template <typename Layout_t, typename... Args>
auto label(typename Label<Layout_t>::Parameters p)
    -> std::unique_ptr<Label<Layout_t>>
{
    return std::make_unique<Label<Layout_t>>(std::move(p));
}

/// Helper function to create an instance.
template <typename... Args>
auto h_label(typename Label<layout::Horizontal<>>::Parameters p)
    -> std::unique_ptr<Label<layout::Horizontal<>>>
{
    return std::make_unique<Label<layout::Horizontal<>>>(std::move(p));
}

/// Helper function to create an instance.
template <typename... Args>
auto v_label(typename Label<layout::Vertical<>>::Parameters p)
    -> std::unique_ptr<Label<layout::Vertical<>>>
{
    return std::make_unique<Label<layout::Vertical<>>>(std::move(p));
}

/// Wraps a Widget_t object with a label on the left.
/** Layout_t is applied to the Label object. */
template <typename Layout_t, typename Widget_t>
class Label_left : public layout::Horizontal<> {
   public:
    Label<Layout_t>& label;
    Widget& padding = this->make_child() | pipe::fixed_width(1);
    Widget_t& wrapped;

   public:
    /// Constructs Label with given parameters, and Widget_t with args...
    template <typename... Args>
    Label_left(typename Label<Layout_t>::Parameters p, Args&&... args)
        : label{this->make_child<Label<Layout_t>>(std::move(p))},
          wrapped{this->make_child<Widget_t>(std::forward<Args>(args)...)}
    {
        this->height_policy = wrapped.height_policy;
        *this | pipe::on_focus_in([this] { System::set_focus(wrapped); });
    }
};

/// Helper function to create an instance.
template <typename Layout_t, typename Widget_t, typename... Args>
auto label_left(typename Label<Layout_t>::Parameters p, Args&&... args)
    -> std::unique_ptr<Label_left<Layout_t, Widget_t>>
{
    return std::make_unique<Label_left<Layout_t, Widget_t>>(
        std::move(p), std::forward<Args>(args)...);
}

/// Wraps a Widget_t object with a label on the right.
/** Layout_t is applied to the Label object. */
template <typename Layout_t, typename Widget_t>
class Label_right : public layout::Horizontal<> {
   public:
    Widget_t& wrapped;
    Widget& padding = this->make_child() | pipe::fixed_width(1);
    Label<Layout_t>& label;

   public:
    /// Constructs Label with \p label_, and Widget_t with args...
    template <typename... Args>
    Label_right(typename Label<Layout_t>::Parameters p, Args&&... args)
        : wrapped{this->make_child<Widget_t>(std::forward<Args>(args)...)},
          label{this->make_child<Label<Layout_t>>(std::move(p))}
    {
        this->height_policy = wrapped.height_policy;
        *this | pipe::on_focus_in([this] { System::set_focus(wrapped); });
    }
};

/// Helper function to create an instance.
template <typename Layout_t, typename Widget_t, typename... Args>
auto label_right(typename Label<Layout_t>::Parameters p, Args&&... args)
    -> std::unique_ptr<Label_right<Layout_t, Widget_t>>
{
    return std::make_unique<Label_right<Layout_t, Widget_t>>(
        std::move(p), std::forward<Args>(args)...);
}

/// Wraps a Widget_t object with a label on the top.
/** Layout_t is applied to the Label object. */
template <typename Layout_t, typename Widget_t>
class Label_top : public layout::Vertical<> {
   public:
    Label<Layout_t>& label;
    Widget& padding = this->make_child() | pipe::fixed_height(0);
    Widget_t& wrapped;

   public:
    /// Constructs Label with \p label_, and Widget_t with args...
    template <typename... Args>
    Label_top(typename Label<Layout_t>::Parameters p, Args&&... args)
        : label{this->make_child<Label<Layout_t>>(std::move(p))},
          wrapped{this->make_child<Widget_t>(std::forward<Args>(args)...)}
    {
        *this | pipe::on_focus_in([this] { System::set_focus(wrapped); });
    }
};

/// Helper function to create an instance.
template <typename Layout_t, typename Widget_t, typename... Args>
auto label_top(typename Label<Layout_t>::Parameters p, Args&&... args)
    -> std::unique_ptr<Label_top<Layout_t, Widget_t>>
{
    return std::make_unique<Label_top<Layout_t, Widget_t>>(
        std::move(p), std::forward<Args>(args)...);
}

/// Wraps a Widget_t object with a label on the bottom.
/** Layout_t is applied to the Label object. */
template <typename Layout_t, typename Widget_t>
class Label_bottom : public layout::Vertical<> {
   public:
    Widget_t& wrapped;
    Widget& padding = this->make_child() | pipe::fixed_height(0);
    Label<Layout_t>& label;

   public:
    /// Constructs Label with \p label_, and Widget_t with args...
    template <typename... Args>
    Label_bottom(typename Label<Layout_t>::Parameters p, Args&&... args)
        : wrapped{this->make_child<Widget_t>(std::forward<Args>(args)...)},
          label{this->make_child<Label>(std::move(p))}
    {
        *this | pipe::on_focus_in([this] { System::set_focus(wrapped); });
    }
};

/// Helper function to create an instance.
template <typename Layout_t, typename Widget_t, typename... Args>
auto label_bottom(typename Label<Layout_t>::Parameters p, Args&&... args)
    -> std::unique_ptr<Label_bottom<Layout_t, Widget_t>>
{
    return std::make_unique<Label_bottom<Layout_t, Widget_t>>(
        std::move(p), std::forward<Args>(args)...);
}

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_LABEL_HPP
