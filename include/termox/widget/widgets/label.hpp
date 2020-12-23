#ifndef TERMOX_WIDGET_WIDGETS_LABEL_HPP
#define TERMOX_WIDGET_WIDGETS_LABEL_HPP
#include <cstddef>
#include <memory>
#include <utility>

#include <termox/painter/glyph_string.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/growth.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>

namespace ox {

/// A single line of text with alignment, non-editable.
/** Can be either Vertical or Horizontal by passing in a Layout type. */
template <template <typename> typename Layout_t>
class Label : public Widget {
    static_assert(layout::is_vertical_v<Layout_t<Widget>> ||
                  layout::is_horizontal_v<Layout_t<Widget>>);

   public:
    struct Parameters {
        Glyph_string text;
        Align alignment         = Align::Left;
        std::size_t extra_left  = 0uL;
        std::size_t extra_right = 0uL;
        Growth growth_strategy  = Growth::Static;
    };

   public:
    /// Create a new Label Widget.
    /** \param text            The initial text used for the display.
     *  \param alignment       The placement of the text on the Label.
     *  \param extra_left      Align::Center takes this into account.
     *  \param extra_right     Align::Center takes this into account
     *  \param growth_strategy If the Label is resized to fit the text or not.
     */
    explicit Label(Glyph_string text       = L"",
                   Align alignment         = Align::Left,
                   std::size_t extra_left  = 0uL,
                   std::size_t extra_right = 0uL,
                   Growth growth_strategy  = Growth::Static)
        : text_{std::move(text)},
          alignment_{alignment},
          extra_left_{extra_left},
          extra_right_{extra_right},
          growth_strategy_{growth_strategy}
    {
        if constexpr (is_vertical)
            *this | pipe::fixed_width(1uL);
        else
            *this | pipe::fixed_height(1uL);
    }

    /// Construct directly with Parameters object.
    explicit Label(Parameters p)
        : Label(std::move(p.text),
                p.alignment,
                p.extra_left,
                p.extra_right,
                p.growth_strategy)
    {}

   public:
    /// Set text contents of Label and update display.
    void set_text(Glyph_string text)
    {
        if (growth_strategy_ == Growth::Dynamic)
            *this | pipe::fixed_width(text.size());
        text_ = std::move(text);
        this->update_offset();
    }

    /// Return the text given to set_text().
    auto get_text() const -> Glyph_string const& { return text_; }

    /// Set text alignment of Label and update display.
    void set_alignment(Align x)
    {
        alignment_ = x;
        this->update_offset();
    }

    /// Return the Align given to set_alignment().
    auto get_alignment() const -> Align { return alignment_; }

    /// Inform Label about space to left of Label for centered text offset.
    void set_extra_left(std::size_t x)
    {
        extra_left_ = x;
        this->update_offset();
    }

    /// Return the amount given to set_extra_left().
    auto get_extra_left() const -> std::size_t { return extra_left_; }

    /// Inform Label about space to right of Label for centered text offset.
    void set_extra_right(std::size_t x)
    {
        extra_right_ = x;
        this->update_offset();
    }

    /// Return the amount given to set_extra_right().
    auto get_extra_right() const -> std::size_t { return extra_right_; }

    /// Enable/Disable Dynamic size, where the Label's size is the text length.
    void set_growth_strategy(Growth type)
    {
        growth_strategy_ = type;
        if (growth_strategy_ == Growth::Dynamic) {
            if constexpr (is_vertical)
                *this | pipe::fixed_height(text_.size());
            else
                *this | pipe::fixed_width(text_.size());
        }
    }

    /// Return the value given to set_growth_strategy().
    auto get_growth_strategy() const -> Growth { return growth_strategy_; }

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
    inline static auto constexpr is_vertical =
        layout::is_vertical_v<Layout_t<Widget>>;

    // Publicly Known Parameters
    Glyph_string text_;
    Align alignment_;
    std::size_t extra_left_;
    std::size_t extra_right_;
    Growth growth_strategy_;

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

        offset_ = this->find_offset(text_.size(), box_length, extra_left_,
                                    extra_right_);
        this->update();
    }

    void paint_vertical()
    {
        auto p = Painter{*this};
        for (auto i = 0uL; i < text_.size() && i < this->height(); ++i)
            p.put(text_[i], {0uL, offset_ + i});
    }

    void paint_horizontal() { Painter{*this}.put(text_, {offset_, 0uL}); }

    /// Find the text's first Glyph placement along the length of the Widget.
    auto find_offset(std::size_t text_length,
                     std::size_t box_length,
                     std::size_t extra_left,
                     std::size_t extra_right) -> std::size_t
    {
        switch (alignment_) {
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
using HLabel = Label<layout::Horizontal>;

/// Vertical Label Widget
using VLabel = Label<layout::Vertical>;

/// Helper function to create an instance.
template <template <typename> typename Layout_t, typename... Args>
auto label(Args&&... args) -> std::unique_ptr<Label<Layout_t>>
{
    return std::make_unique<Label<Layout_t>>(std::forward(args)...);
}

/// Helper function to create an instance.
template <typename... Args>
auto hlabel(Args&&... args) -> std::unique_ptr<Label<layout::Horizontal>>
{
    return std::make_unique<Label<layout::Horizontal>>(
        std::forward<Args>(args)...);
}

/// Helper function to create an instance.
template <typename... Args>
auto vlabel(Args&&... args) -> std::unique_ptr<Label<layout::Vertical>>
{
    return std::make_unique<Label<layout::Vertical>>(
        std::forward<Args>(args)...);
}

/// Wraps a Widget_t object with a label on the left.
/** Layout_t is applied to the Label object. */
template <template <typename> typename Layout_t, typename Widget_t>
class Label_left : public layout::Horizontal<> {
   public:
    Label<Layout_t>& label;
    Widget& padding = make_child() | pipe::fixed_width(1);
    Widget_t& wrapped;

   public:
    /// Construct a new Label and wrapped Widget_t.
    /** Only takes Label constructor args, if you need to pass in args to the
     *  wrapped Widget_t, then use the Label::Parameters overload. */
    explicit Label_left(Glyph_string text       = L"",
                        Align alignment         = Align::Left,
                        std::size_t extra_left  = 0uL,
                        std::size_t extra_right = 0uL,
                        Growth growth_strategy  = Growth::Static)
        : Label_left{typename Label<Layout_t>::Parameters{
              std::move(text), alignment, extra_left, extra_right,
              growth_strategy}}
    {}

    /// Constructs Label with given parameters, and Widget_t with args...
    template <typename... Args>
    explicit Label_left(typename Label<Layout_t>::Parameters p, Args&&... args)
        : label{make_child<Label<Layout_t>>(std::move(p))},
          wrapped{make_child<Widget_t>(std::forward<Args>(args)...)}
    {
        this->height_policy = wrapped.height_policy;
        *this | pipe::on_focus_in([this] { System::set_focus(wrapped); });
    }
};

/// Helper function to create an instance.
template <template <typename> typename Layout_t,
          typename Widget_t,
          typename... Args>
auto label_left(typename Label<Layout_t>::Parameters p, Args&&... args)
    -> std::unique_ptr<Label_left<Layout_t, Widget_t>>
{
    return std::make_unique<Label_left<Layout_t, Widget_t>>(
        std::move(p), std::forward<Args>(args)...);
}

/// Helper function to create an instance.
/** Only takes Label constructor args, if you need to pass in args to the
 *  wrapped Widget_t, then use the Label::Parameters overload. */
template <template <typename> typename Layout_t, typename Widget_t>
auto label_left(Glyph_string text       = L"",
                Align alignment         = Align::Left,
                std::size_t extra_left  = 0uL,
                std::size_t extra_right = 0uL,
                Growth growth_strategy  = Growth::Static)
    -> std::unique_ptr<Label_left<Layout_t, Widget_t>>
{
    return label_left(typename Label<Layout_t>::Parameters{
        std::move(text), alignment, extra_left, extra_right, growth_strategy});
}

/// Wraps a Widget_t object with a label on the right.
/** Layout_t is applied to the Label object. */
template <template <typename> typename Layout_t, typename Widget_t>
class Label_right : public layout::Horizontal<> {
   public:
    Widget_t& wrapped;
    Widget& padding = make_child() | pipe::fixed_width(1);
    Label<Layout_t>& label;

   public:
    /// Construct a new Label and wrapped Widget_t.
    /** Only takes Label constructor args, if you need to pass in args to the
     *  wrapped Widget_t, then use the Label::Parameters overload. */
    explicit Label_right(Glyph_string text       = L"",
                         Align alignment         = Align::Left,
                         std::size_t extra_left  = 0uL,
                         std::size_t extra_right = 0uL,
                         Growth growth_strategy  = Growth::Static)
        : Label_right{typename Label<Layout_t>::Parameters{
              std::move(text), alignment, extra_left, extra_right,
              growth_strategy}}
    {}

    /// Constructs Label with \p label_, and Widget_t with args...
    template <typename... Args>
    explicit Label_right(typename Label<Layout_t>::Parameters p, Args&&... args)
        : wrapped{make_child<Widget_t>(std::forward<Args>(args)...)},
          label{make_child<Label<Layout_t>>(std::move(p))}
    {
        this->height_policy = wrapped.height_policy;
        *this | pipe::on_focus_in([this] { System::set_focus(wrapped); });
    }
};

/// Helper function to create an instance.
template <template <typename> typename Layout_t,
          typename Widget_t,
          typename... Args>
auto label_right(typename Label<Layout_t>::Parameters p, Args&&... args)
    -> std::unique_ptr<Label_right<Layout_t, Widget_t>>
{
    return std::make_unique<Label_right<Layout_t, Widget_t>>(
        std::move(p), std::forward<Args>(args)...);
}

/// Helper function to create an instance.
/** Only takes Label constructor args, if you need to pass in args to the
 *  wrapped Widget_t, then use the Label::Parameters overload. */
template <template <typename> typename Layout_t, typename Widget_t>
auto label_right(Glyph_string text       = L"",
                 Align alignment         = Align::Left,
                 std::size_t extra_left  = 0uL,
                 std::size_t extra_right = 0uL,
                 Growth growth_strategy  = Growth::Static)
    -> std::unique_ptr<Label_left<Layout_t, Widget_t>>
{
    return label_right(typename Label<Layout_t>::Parameters{
        std::move(text), alignment, extra_left, extra_right, growth_strategy});
}

/// Wraps a Widget_t object with a label on the top.
/** Layout_t is applied to the Label object. */
template <template <typename> typename Layout_t, typename Widget_t>
class Label_top : public layout::Vertical<> {
   public:
    Label<Layout_t>& label;
    Widget& padding = make_child() | pipe::fixed_height(0);
    Widget_t& wrapped;

   public:
    /// Construct a new Label and wrapped Widget_t.
    /** Only takes Label constructor args, if you need to pass in args to the
     *  wrapped Widget_t, then use the Label::Parameters overload. */
    explicit Label_top(Glyph_string text        = L"",
                       Align alignment          = Align::Left,
                       std::size_t extra_top    = 0uL,
                       std::size_t extra_bottom = 0uL,
                       Growth growth_strategy   = Growth::Static)
        : Label_top{typename Label<Layout_t>::Parameters{
              std::move(text), alignment, extra_top, extra_bottom,
              growth_strategy}}
    {}

    /// Constructs Label with \p label_, and Widget_t with args...
    template <typename... Args>
    explicit Label_top(typename Label<Layout_t>::Parameters p, Args&&... args)
        : label{make_child<Label<Layout_t>>(std::move(p))},
          wrapped{make_child<Widget_t>(std::forward<Args>(args)...)}
    {
        *this | pipe::on_focus_in([this] { System::set_focus(wrapped); });
    }
};

/// Helper function to create an instance.
template <template <typename> typename Layout_t,
          typename Widget_t,
          typename... Args>
auto label_top(typename Label<Layout_t>::Parameters p, Args&&... args)
    -> std::unique_ptr<Label_top<Layout_t, Widget_t>>
{
    return std::make_unique<Label_top<Layout_t, Widget_t>>(
        std::move(p), std::forward<Args>(args)...);
}

/// Helper function to create an instance.
/** Only takes Label constructor args, if you need to pass in args to the
 *  wrapped Widget_t, then use the Label::Parameters overload. */
template <template <typename> typename Layout_t, typename Widget_t>
auto label_top(Glyph_string text        = L"",
               Align alignment          = Align::Left,
               std::size_t extra_top    = 0uL,
               std::size_t extra_bottom = 0uL,
               Growth growth_strategy   = Growth::Static)
    -> std::unique_ptr<Label_left<Layout_t, Widget_t>>
{
    return label_top(typename Label<Layout_t>::Parameters{
        std::move(text), alignment, extra_top, extra_bottom, growth_strategy});
}

/// Wraps a Widget_t object with a label on the bottom.
/** Layout_t is applied to the Label object. */
template <template <typename> typename Layout_t, typename Widget_t>
class Label_bottom : public layout::Vertical<> {
   public:
    Widget_t& wrapped;
    Widget& padding = make_child() | pipe::fixed_height(0);
    Label<Layout_t>& label;

   public:
    /// Construct a new Label and wrapped Widget_t.
    /** Only takes Label constructor args, if you need to pass in args to the
     *  wrapped Widget_t, then use the Label::Parameters overload. */
    explicit Label_bottom(Glyph_string text        = L"",
                          Align alignment          = Align::Left,
                          std::size_t extra_top    = 0uL,
                          std::size_t extra_bottom = 0uL,
                          Growth growth_strategy   = Growth::Static)
        : Label_bottom{typename Label<Layout_t>::Parameters{
              std::move(text), alignment, extra_top, extra_bottom,
              growth_strategy}}
    {}

    /// Constructs Label with \p label_, and Widget_t with args...
    template <typename... Args>
    explicit Label_bottom(typename Label<Layout_t>::Parameters p,
                          Args&&... args)
        : wrapped{make_child<Widget_t>(std::forward<Args>(args)...)},
          label{make_child<Label>(std::move(p))}
    {
        *this | pipe::on_focus_in([this] { System::set_focus(wrapped); });
    }
};

/// Helper function to create an instance.
template <template <typename> typename Layout_t,
          typename Widget_t,
          typename... Args>
auto label_bottom(typename Label<Layout_t>::Parameters p, Args&&... args)
    -> std::unique_ptr<Label_bottom<Layout_t, Widget_t>>
{
    return std::make_unique<Label_bottom<Layout_t, Widget_t>>(
        std::move(p), std::forward<Args>(args)...);
}

/// Helper function to create an instance.
/** Only takes Label constructor args, if you need to pass in args to the
 *  wrapped Widget_t, then use the Label::Parameters overload. */
template <template <typename> typename Layout_t, typename Widget_t>
auto label_bottom(Glyph_string text        = L"",
                  Align alignment          = Align::Left,
                  std::size_t extra_top    = 0uL,
                  std::size_t extra_bottom = 0uL,
                  Growth growth_strategy   = Growth::Static)
    -> std::unique_ptr<Label_left<Layout_t, Widget_t>>
{
    return label_bottom(typename Label<Layout_t>::Parameters{
        std::move(text), alignment, extra_top, extra_bottom, growth_strategy});
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_LABEL_HPP
