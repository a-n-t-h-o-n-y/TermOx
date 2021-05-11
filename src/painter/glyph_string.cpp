#include <termox/painter/glyph_string.hpp>

#include <algorithm>
#include <string>
#include <vector>

#include <termox/common/u32_to_mb.hpp>
#include <termox/painter/brush.hpp>
#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/trait.hpp>

namespace ox {

Glyph_string::Glyph_string(Glyph glyph, int count)
{
    for (; count != 0; --count)
        this->append(glyph);
}

auto Glyph_string::append(Glyph g) -> Glyph_string&
{
    this->push_back(g);
    return *this;
}

auto Glyph_string::append(Glyph_string const& gs) -> Glyph_string&
{
    for (Glyph g : gs)
        this->append(g);
    return *this;
}

auto Glyph_string::length() const -> int { return this->size(); }

auto Glyph_string::size() const -> int { return this->vector::size(); }

auto Glyph_string::u32str() const -> std::u32string
{
    auto result = std::u32string{};
    result.reserve(this->size());
    for (Glyph g : *this)
        result.push_back(g.symbol);
    return result;
}

auto Glyph_string::str() const -> std::string
{
    return u32_to_mb(this->u32str());
}

void Glyph_string::add_traits(Traits traits)
{
    for (auto& glyph : *this)
        glyph.brush.traits.insert(traits);
}

void Glyph_string::clear_traits()
{
    for (auto& glyph : *this)
        glyph.brush.traits = Trait::None;
}

void Glyph_string::add_color(Background_color bg)
{
    for (auto& glyph : *this)
        glyph.brush.background = Color{bg.value};
}

void Glyph_string::add_color(Foreground_color fg)
{
    for (auto& glyph : *this)
        glyph.brush.foreground = Color{fg.value};
}

void Glyph_string::remove_traits(Traits traits)
{
    for (Glyph& glyph : *this)
        glyph.brush.traits.remove(traits);
}

void Glyph_string::remove_background()
{
    for (auto& glyph : *this)
        glyph.brush.background = Color::Background;
}

void Glyph_string::remove_foreground()
{
    for (auto& glyph : *this)
        glyph.brush.foreground = Color::Foreground;
}

auto operator|(Glyph_string& gs, Traits ts) -> Glyph_string&
{
    gs.add_traits(ts);
    return gs;
}

auto operator|(Glyph_string const& gs, Traits ts) -> Glyph_string
{
    auto copy = gs;
    copy.add_traits(ts);
    return copy;
}

auto operator|(Glyph_string&& gs, Traits ts) -> Glyph_string { return gs | ts; }

}  // namespace ox

namespace esc {

auto operator|(char32_t const* gs, Trait t) -> ox::Glyph_string
{
    return ox::Glyph_string{gs} | t;
}

auto operator|(char32_t const* gs, Traits ts) -> ox::Glyph_string
{
    return ox::Glyph_string{gs} | ts;
}

auto operator|(char const* gs, Trait t) -> ox::Glyph_string
{
    return ox::Glyph_string{gs} | t;
}

auto operator|(char const* gs, Traits ts) -> ox::Glyph_string
{
    return ox::Glyph_string{gs} | ts;
}
}  // namespace esc

namespace ox {

auto operator|(Glyph_string& gs, Background_color c) -> Glyph_string&
{
    gs.add_color(c);
    return gs;
}

auto operator|(Glyph_string const& gs, Background_color c) -> Glyph_string
{
    auto copy = gs;
    copy.add_color(c);
    return copy;
}

auto operator|(Glyph_string&& gs, Background_color c) -> Glyph_string
{
    return gs | c;
}

auto operator|(char32_t const* gs, Background_color c) -> Glyph_string
{
    return Glyph_string{gs} | c;
}

auto operator|(Glyph_string& gs, Foreground_color c) -> Glyph_string&
{
    gs.add_color(c);
    return gs;
}

auto operator|(Glyph_string const& gs, Foreground_color c) -> Glyph_string
{
    auto copy = gs;
    copy.add_color(c);
    return copy;
}

auto operator|(Glyph_string&& gs, Foreground_color c) -> Glyph_string
{
    return gs | c;
}

auto operator|(char32_t const* gs, Foreground_color c) -> Glyph_string
{
    return Glyph_string{gs} | c;
}

auto operator|(Glyph_string& gs, Brush b) -> Glyph_string&
{
    for (auto& g : gs)
        g.brush = b;
    return gs;
}

auto operator|(Glyph_string const& gs, Brush b) -> Glyph_string
{
    auto copy = gs;
    for (auto& g : copy)
        g.brush = b;
    return copy;
}

auto operator|(Glyph_string&& gs, Brush b) -> Glyph_string { return gs | b; }

auto operator|(char32_t const* gs, Brush b) -> Glyph_string
{
    return Glyph_string{gs} | b;
}

auto operator==(Glyph_string const& x, Glyph_string const& y) -> bool
{
    return std::equal(std::begin(x), std::end(x), std::begin(y), std::end(y));
}

auto operator!=(Glyph_string const& x, Glyph_string const& y) -> bool
{
    return !(x == y);
}

}  // namespace ox
