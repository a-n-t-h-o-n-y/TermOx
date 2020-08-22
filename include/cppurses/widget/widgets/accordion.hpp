#ifndef CPPURSES_WIDGET_WIDGETS_ACCORDION_HPP
#define CPPURSES_WIDGET_WIDGETS_ACCORDION_HPP
#include <memory>
#include <utility>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/detail/linear_layout.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/opposite.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widgets/text_display.hpp>

namespace cppurses {
/// Layout_t is the Layout to use for the Bar, not Accordion.
template <typename Layout_t>
class Bar : public Layout_t {
   private:
    class Indicator : public Widget {
       public:
        Indicator()
        {
            using namespace pipe;
            if constexpr (layout::is_vertical_v<Layout_t>)
                *this | fixed_height(1uL);
            else
                *this | fixed_width(3uL);
        }

       public:
        void plus()
        {
            x_ = L"[+]";
            this->update();
        }

        void minus()
        {
            x_ = L"[-]";
            this->update();
        }

       protected:
        auto paint_event() -> bool override
        {
            Painter{*this}.put(x_, {0uL, 0uL});
            return Widget::paint_event();
        }

       private:
        Glyph_string x_ = L"[+]";
    };

    class Buffer : public Widget {
       public:
        Buffer()
        {
            using namespace pipe;
            if constexpr (layout::is_vertical_v<Layout_t>)
                *this | fixed_height(length_);
            else
                *this | fixed_width(length_);
        }

       private:
        inline static constexpr auto length_ = 3uL;
    };

    // TODO make a text display type widget that centers the text, working with
    // the Layout_t, if Horizontal, then it just finds the offset of center and
    // places the text, if it is vertical then it loops through the glyph string
    // and finds the center offset and places glyphs one at a time, incrementing
    // the y coordinate.
    class Title_line : public layout::Opposite_t<Layout_t> {
       public:
        Title_line(Glyph_string title)
        {
            using namespace cppurses::pipe;
            if constexpr (layout::is_vertical_v<layout::Opposite_t<Layout_t>>) {
                this->template make_child<Text_display>(std::move(title)) |
                    fixed_height(1uL);
            }
            else {
                this->template make_child<Widget>();
                this->template make_child<Text_display>(std::move(title));
                this->template make_child<Widget>();
                *this | children() | fixed_width(1);
            }
        }
    };

   public:
    sig::Signal<void()> toggle_request;

   public:
    Bar(Glyph_string title)
        : text_{this->template make_child<Title_line>(std::move(title))}
    {
        using namespace pipe;
        if constexpr (layout::is_vertical_v<Layout_t>)
            *this | fixed_width(3uL);
        else
            *this | fixed_height(1uL);
        this->collapse();
        indicator_.mouse_pressed.connect(
            [this](auto const&) { this->toggle_request(); });

        // *this | descendants() | bg(Color::Gray);
    }

   public:
    void expand() { indicator_.minus(); }

    void collapse() { indicator_.plus(); }

   private:
    Indicator& indicator_ = this->template make_child<Indicator>();
    Buffer& buffer_       = this->template make_child<Buffer>();
    Title_line& text_;
};

enum class Bar_position { First, Last };

template <typename Layout_t,
          typename Widget_t,
          Bar_position position = Bar_position::First>
class Accordion : public Layout_t {
   private:
    using Bar_t = Bar<layout::Opposite_t<Layout_t>>;

   public:
    template <typename... Args>
    Accordion(Glyph_string title, Args&&... args)
        : bar_{this->template make_child<Bar_t>(std::move(title))},
          wrapped_{
              this->template make_child<Widget_t>(std::forward<Args>(args)...)}
    {
        if constexpr (wrapped_index_ == 0uL)
            this->Layout::swap(0uL, 1uL);

        if constexpr (layout::is_vertical_v<Layout_t>)
            *this | pipe::passive_height();
        else
            *this | pipe::passive_width();

        bar_.toggle_request.connect([this] { this->toggle_expansion(); });

        this->collapse();
    }

   public:
    /// Enable the wrapped Widget.
    void expand()
    {
        expanded_ = true;
        bar_.expand();
        this->reinsert_wrapped();
        this->enable(this->is_enabled(), false);
    }

    /// Disable the wrapped Widget.
    void collapse()
    {
        expanded_ = false;
        bar_.collapse();
        this->extract_wrapped();
        this->enable(this->is_enabled(), false);
    }

    void toggle_expansion()
    {
        if (expanded_)
            this->collapse();
        else
            this->expand();
    }

    /// Return the wrapped widget.
    auto wrapped() -> Widget_t& { return wrapped_; }

   private:
    Bar_t& bar_;
    Widget_t& wrapped_;

    bool expanded_                     = true;
    std::unique_ptr<Widget> w_storage_ = nullptr;

    inline static constexpr auto wrapped_index_ =
        position == Bar_position::First ? 1uL : 0uL;

   private:
    void reinsert_wrapped()
    {
        if (w_storage_ == nullptr)
            return;
        this->Layout::insert(std::move(w_storage_), wrapped_index_);
        w_storage_ = nullptr;
    }

    void extract_wrapped()
    {
        if (w_storage_ == nullptr)
            w_storage_ = this->Layout::remove(wrapped_index_);
    }
};

template <typename Layout_t,
          typename Widget_t,
          Bar_position position = Bar_position::First,
          typename... Args>
auto accordion(Args&&... args)
    -> std::unique_ptr<Accordion<Layout_t, Widget_t, position>>
{
    return std::make_unique<Accordion<Layout_t, Widget_t, position>>(
        std::forward<Args>(args)...);
}

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_ACCORDION_HPP
