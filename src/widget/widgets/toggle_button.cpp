#include <termox/widget/widgets/toggle_button.hpp>

#include <memory>
#include <utility>

#include <termox/painter/glyph_string.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widgets/button.hpp>

namespace {

auto constexpr top_index    = 0uL;
auto constexpr bottom_index = 1uL;

}  // namespace

namespace ox {

Toggle_button::Toggle_button(Glyph_string top_text, Glyph_string bottom_text)
    : SPair<Button, Button>{{std::move(top_text)}, {std::move(bottom_text)}}
{
    using namespace ox::pipe;
    *this | active_page(::top_index);
    this->give_focus_on_change(false);

    top | on_press([this]() { *this | active_page(::bottom_index); });
    bottom | on_press([this]() { *this | active_page(::top_index); });
}

Toggle_button::Toggle_button(Parameters p)
    : Toggle_button{std::move(p.top_text), std::move(p.bottom_text)}
{}

void Toggle_button::show_top() { this->set_active_page(::top_index); }

void Toggle_button::show_bottom() { this->set_active_page(::bottom_index); }

void Toggle_button::toggle()
{
    if (this->active_page_index() == ::top_index)
        this->set_active_page(::bottom_index);
    else
        this->set_active_page(::top_index);
}

auto toggle_button(Glyph_string top_text, Glyph_string bottom_text)
    -> std::unique_ptr<Toggle_button>
{
    return std::make_unique<Toggle_button>(std::move(top_text),
                                           std::move(bottom_text));
}

auto toggle_button(Toggle_button::Parameters p)
    -> std::unique_ptr<Toggle_button>
{
    return std::make_unique<Toggle_button>(std::move(p));
}

}  // namespace ox
