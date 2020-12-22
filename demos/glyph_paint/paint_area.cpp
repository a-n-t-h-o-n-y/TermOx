#include "paint_area.hpp"

#include <cctype>
#include <codecvt>
#include <cstddef>
#include <cstdint>
#include <cwctype>
#include <iostream>
#include <iterator>
#include <locale>
#include <optional>
#include <string>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/brush.hpp>
#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/painter/trait.hpp>
#include <termox/system/key.hpp>
#include <termox/widget/border.hpp>
#include <termox/widget/detail/link_lifetimes.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

using namespace ox;

namespace {

void insert_newline(Point first, Point second, std::ostream& os)
{
    if (first.y >= second.y)
        return;
    os << std::string(second.y - first.y, '\n');
}

void insert_space(Point first, Point second, std::ostream& os)
{
    auto spaces_n = second.x;
    if (first.y == second.y)
        spaces_n -= first.x + 1;
    os << std::string(spaces_n, ' ');
}

}  // namespace

namespace paint {

void Paint_area::write(std::ostream& os)
{
    auto previous_nl = Point{0, 0};
    auto previous_s  = Point{0, static_cast<std::size_t>(-1)};
    for (auto const& [point, glyph] : glyphs_painted_) {
        insert_newline(previous_nl, point, os);
        insert_space(previous_s, point, os);
        os << ox::utility::wchar_to_bytes(glyph.symbol);
        previous_nl = point;
        previous_s  = point;
    }
}

void Paint_area::read(std::istream& is)
{
    this->clear();
    auto at = Point{0, 0};
    is >> std::noskipws;
    auto const text = Glyph_string{std::istream_iterator<char>{is},
                                   std::istream_iterator<char>()};
    for (auto const& glyph : text) {
        if (!std::iswspace(glyph.symbol))
            glyphs_painted_[at] = glyph;
        ++at.x;
        if (glyph.symbol == L'\n') {
            ++at.y;
            at.x = 0uL;
        }
    }
}

auto Paint_area::key_press_event(Key k) -> bool
{
    auto const symbol = to_wchar(k);
    if (!std::iscntrl(symbol))
        this->set_symbol(symbol);
    auto const w = this->width();
    auto const h = this->height();
    if (!this->cursor.enabled() || w == 0 || h == 0)
        return Widget::key_press_event(k);
    auto new_x = this->cursor.x() + 1uL;
    auto new_y = this->cursor.y() + 1uL;
    switch (k) {
        case Key::Arrow_right:
            if (new_x == w)
                new_x = 0;
            this->cursor.set_x(new_x);
            break;
        case Key::Arrow_left: this->cursor.set_x(this->cursor.x() - 1); break;
        case Key::Arrow_down:
            if (new_y == h)
                new_y = 0;
            this->cursor.set_y(new_y);
            break;
        case Key::Arrow_up: this->cursor.set_y(this->cursor.y() - 1); break;
        case Key::Enter:
            this->place_glyph(this->cursor.x(), this->cursor.y());
            break;
        default:
            if (!std::iscntrl(symbol)) {
                this->place_glyph(this->cursor.x(), this->cursor.y());
                this->update();
            }
            break;
    }
    return Widget::key_press_event(k);
}

void Paint_area::place_glyph(std::size_t x, std::size_t y)
{
    if (clone_enabled_) {
        if (glyphs_painted_.count({x, y}) == 1) {
            this->set_glyph(glyphs_painted_[{x, y}]);
            this->toggle_clone();
        }
    }
    else if (erase_enabled_)
        this->remove_glyph({x, y});
    else {
        glyphs_painted_[{x, y}] = current_glyph_;
        this->update();
    }
}

}  // namespace paint

namespace paint::slot {

using ox::slot::link_lifetimes;

auto set_symbol(Paint_area& pa) -> sl::Slot<void(Glyph)>
{
    return link_lifetimes(
        [&pa](Glyph symbol) {
            if (symbol.symbol != L' ')
                pa.set_symbol(std::move(symbol));
        },
        pa);
}

auto set_foreground_color(Paint_area& pa) -> sl::Slot<void(Color)>
{
    return link_lifetimes([&pa](Color c) { pa.set_foreground_color(c); }, pa);
}

auto set_background_color(Paint_area& pa) -> sl::Slot<void(Color)>
{
    return link_lifetimes([&pa](Color c) { pa.set_background_color(c); }, pa);
}

auto set_trait(Paint_area& pa, Trait t) -> sl::Slot<void()>
{
    return link_lifetimes([&pa, t] { pa.set_trait(t); }, pa);
}

auto remove_traits(Paint_area& pa, Trait t) -> sl::Slot<void()>
{
    return link_lifetimes([&pa, t] { pa.remove_traits(t); }, pa);
}

auto toggle_clone(Paint_area& pa) -> sl::Slot<void()>
{
    return link_lifetimes([&pa] { pa.toggle_clone(); }, pa);
}

auto clear(Paint_area& pa) -> sl::Slot<void()>
{
    return link_lifetimes([&pa] { pa.clear(); }, pa);
}

auto enable_erase(Paint_area& pa) -> sl::Slot<void()>
{
    return link_lifetimes([&pa] { pa.enable_erase(); }, pa);
}

auto disable_erase(Paint_area& pa) -> sl::Slot<void()>
{
    return link_lifetimes([&pa] { pa.disable_erase(); }, pa);
}

auto enable_grid(Paint_area& pa) -> sl::Slot<void()>
{
    return link_lifetimes([&pa] { pa.enable_grid(); }, pa);
}

auto disable_grid(Paint_area& pa) -> sl::Slot<void()>
{
    return link_lifetimes([&pa] { pa.disable_grid(); }, pa);
}

auto write(Paint_area& pa) -> sl::Slot<void(std::ostream&)>
{
    return link_lifetimes([&pa](std::ostream& os) { pa.write(os); }, pa);
}

auto read(Paint_area& pa) -> sl::Slot<void(std::istream&)>
{
    return link_lifetimes([&pa](std::istream& is) { pa.read(is); }, pa);
}

}  // namespace paint::slot
