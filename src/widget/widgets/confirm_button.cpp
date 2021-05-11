#include <termox/widget/widgets/confirm_button.hpp>

#include <memory>
#include <utility>

#include <termox/painter/color.hpp>
#include <termox/widget/array.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/tuple.hpp>
#include <termox/widget/widgets/button.hpp>

namespace ox::detail {

Confirm_page::Confirm_page(Glyph_string confirm_text)
    : HArray<Button, 2>{{Button::Parameters{std::move(confirm_text)}, {U"X"}}}
{
    using namespace pipe;
    confirm_btn | bg(Color::Blue) | fg(Color::Black);
    exit_btn | bg(Color::Gray) | fg(Color::Black) | fixed_width(3);
}

Confirm_page::Confirm_page(Parameters p)
    : Confirm_page{std::move(p.confirm_text)}
{}

}  // namespace ox::detail

namespace ox {

Confirm_button::Confirm_button(Glyph_string label, Glyph_string confirm_text)
    : STuple<Button, detail::Confirm_page>{{std::move(label)},
                                           {std::move(confirm_text)}}
{
    using namespace pipe;
    *this | fixed_height(1) | active_page(front_page_);
    this->give_focus_on_change(false);

    main_btn | on_left_click([this] { *this | active_page(confirm_page_); });

    confirm_page.confirm_btn | on_left_click([this] {
        pressed.emit();
        *this | active_page(front_page_);
    });

    confirm_page.exit_btn |
        on_left_click([this] { *this | active_page(front_page_); });
}

Confirm_button::Confirm_button(Parameters p)
    : Confirm_button{std::move(p.label), std::move(p.confirm_text)}
{}

auto confirm_button(Glyph_string label, Glyph_string confirm_text)
    -> std::unique_ptr<Confirm_button>
{
    return std::make_unique<Confirm_button>(std::move(label),
                                            std::move(confirm_text));
}

auto confirm_button(Confirm_button::Parameters p)
    -> std::unique_ptr<Confirm_button>
{
    return std::make_unique<Confirm_button>(std::move(p));
}

}  // namespace ox
