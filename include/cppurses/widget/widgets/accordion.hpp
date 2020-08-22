#ifndef CPPURSES_WIDGET_WIDGETS_ACCORDION_HPP
#define CPPURSES_WIDGET_WIDGETS_ACCORDION_HPP
#include <memory>
#include <utility>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/painter/trait.hpp>
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

    class Centered_text : public Widget {
       public:
        Centered_text(Glyph_string text) : text_{L' ' + std::move(text) + L' '}
        {
            if constexpr (layout::is_vertical_v<Layout_t>)
                *this | pipe::wallpaper(L'│' | foreground(Color::Dark_gray));
            else
                *this | pipe::wallpaper(L'─' | foreground(Color::Dark_gray));
        }

       protected:
        auto paint_event() -> bool override
        {
            auto p = Painter{*this};
            if constexpr (layout::is_vertical_v<Layout_t>) {
                for (auto i = 0uL; i < text_.size(); ++i)
                    p.put(text_[i], {0uL, offset_ + i});
            }
            else
                p.put(text_, {offset_, 0uL});
            return Widget::paint_event();
        }

        auto resize_event(Area new_size, Area old_size) -> bool override
        {
            auto length = std::size_t{};
            if constexpr (layout::is_vertical_v<Layout_t>)
                length = this->height();
            else
                length = this->width();
            offset_ = center_offset(text_.length(), length + extra_length_);
            return Widget::resize_event(new_size, old_size);
        }

       private:
        Glyph_string const text_;
        std::size_t offset_ = 0uL;

        // Indicator Length / 2
        inline static auto constexpr extra_length_ =
            layout::is_vertical_v<Layout_t> ? 0uL : 1uL;

       private:
        static auto center_offset(std::size_t string_length,
                                  std::size_t total_length)
        {
            if (string_length > total_length)
                return 0uL;
            else
                return (total_length / 2) - (string_length / 2);
        }
    };

    class Title_line : public layout::Opposite_t<Layout_t> {
       public:
        sig::Signal<void()> clicked;

       public:
        Title_line(Glyph_string title)
        {
            title | Trait::Bold;
            using namespace cppurses::pipe;
            if constexpr (layout::is_vertical_v<layout::Opposite_t<Layout_t>>) {
                this->template make_child<Centered_text>(std::move(title)) |
                    fixed_height(1uL);
            }
            else {
                this->template make_child<Widget>() |
                    on_mouse_press([this](auto const&) { this->clicked(); });
                this->template make_child<Centered_text>(std::move(title)) |
                    on_mouse_press([this](auto const&) { this->clicked(); });
                this->template make_child<Widget>() |
                    on_mouse_press([this](auto const&) { this->clicked(); });
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
        text_.clicked.connect([this] { this->toggle_request(); });
    }

   public:
    void expand() { indicator_.minus(); }

    void collapse() { indicator_.plus(); }

   private:
    Indicator& indicator_ = this->template make_child<Indicator>();
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
            this->Layout_t::swap(0uL, 1uL);

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
        this->Layout_t::insert(std::move(w_storage_), wrapped_index_);
        w_storage_ = nullptr;
    }

    void extract_wrapped()
    {
        if (w_storage_ == nullptr)
            w_storage_ = this->Layout_t::remove(wrapped_index_);
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
