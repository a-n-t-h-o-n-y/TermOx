#include <ox/dotgrid.hpp>
#include <algorithm>
#include <limits>
#include <stdexcept>
#include <utility>

namespace ox {
namespace {
auto extent(Area size) -> Area
{
    validate(size);
    return {size.width / 2 + (size.width % 2 != 0),
            size.height / 4 + (size.height % 4 != 0)};
}
auto bit(Point at) -> std::uint8_t
{
    // Unicode Braille numbering: left 1,2,3,7; right 4,5,6,8.
    constexpr int bits[2][4] = {{0, 1, 2, 6}, {3, 4, 5, 7}};
    return std::uint8_t(1u << bits[at.x % 2][at.y % 4]);
}
void check_colors(DotColors colors)
{ validate(Style{.background = colors.background, .foreground = colors.foreground}); }
auto glyph(std::uint8_t dots) -> char32_t { return dots ? U'\u2800' + dots : U' '; }
}
DotGrid::DotGrid(Area size, DotColors colors) { reset(size, colors); }
DotGrid::DotGrid(DotGrid&& other)
    : Widget{std::move(other)}, size_{std::exchange(other.size_, Area{})},
      cells_{std::move(other.cells_)} { other.cells_.clear(); }
auto DotGrid::operator=(DotGrid&& other) -> DotGrid&
{
    if (this == &other) return *this;
    Widget::operator=(std::move(other));
    size_ = std::exchange(other.size_, Area{});
    cells_ = std::move(other.cells_);
    other.cells_.clear();
    request_repaint();
    return *this;
}
auto DotGrid::cell_size() const -> Area { return extent(size_); }
auto DotGrid::index(Point cell) const -> std::size_t
{
    auto size = cell_size();
    if (!contains({{}, size}, cell)) throw std::out_of_range{"DotGrid cell coordinate"};
    return std::size_t(cell.y) * std::size_t(size.width) + std::size_t(cell.x);
}
void DotGrid::check_dot(Point at) const
{
    if (!contains({{}, size_}, at)) throw std::out_of_range{"DotGrid dot coordinate"};
}
auto DotGrid::dot(Point at) const -> bool
{
    check_dot(at);
    return (cells_[index({at.x / 2, at.y / 4})].dots & bit(at)) != 0;
}
auto DotGrid::set_dot(Point at, bool on) -> bool
{
    check_dot(at);
    auto& mask = cells_[index({at.x / 2, at.y / 4})].dots;
    auto next = std::uint8_t(on ? mask | bit(at) : mask & ~bit(at));
    if (std::exchange(mask, next) == next) return false;
    request_repaint();
    return true;
}
auto DotGrid::cell_colors(Point cell) const -> DotColors { return cells_[index(cell)].colors; }
auto DotGrid::set_cell_colors(Point cell, DotColors colors) -> bool
{
    auto i = index(cell);
    check_colors(colors);
    if (std::exchange(cells_[i].colors, colors) == colors) return false;
    request_repaint();
    return true;
}
auto DotGrid::clear() -> bool
{
    bool changed = false;
    for (auto& cell : cells_) changed |= std::exchange(cell.dots, 0) != 0;
    if (changed) request_repaint();
    return changed;
}
void DotGrid::reset(Area size, DotColors colors)
{
    auto cells = extent(size);
    check_colors(colors);
    auto width = std::size_t(cells.width), height = std::size_t(cells.height);
    auto max = std::size_t(std::numeric_limits<std::ptrdiff_t>::max()) / sizeof(Cell);
    if (height && width > max / height) throw std::length_error{"DotGrid storage overflow"};
    auto data = std::vector<Cell>(width * height, Cell{0, colors});
    request_layout();
    size_ = size;
    cells_.swap(data);
    request_repaint();
}
auto DotGrid::measure(Area available, esc::WidthPolicy) const -> Area
{ validate(available); return cell_size(); }
auto DotGrid::paint(Canvas canvas) -> Cursor
{
    auto clip = intersection(canvas.visible(), {{}, cell_size()});
    // Check each used glyph, including scalar-specific width overrides, before writing.
    for (int y = clip.at.y; y < clip.at.y + clip.size.height; ++y)
        for (int x = clip.at.x; x < clip.at.x + clip.size.width; ++x) {
            auto c = glyph(cells_[index({x, y})].dots);
            if (esc::grapheme_width(std::u32string_view{&c, 1}, canvas.width_policy()) !=
                std::optional<std::size_t>{1})
                throw std::invalid_argument{"DotGrid requires one-column Braille glyphs"};
        }
    for (int y = clip.at.y; y < clip.at.y + clip.size.height; ++y)
        for (int x = clip.at.x; x < clip.at.x + clip.size.width; ++x) {
            auto const& cell = cells_[index({x, y})];
            auto c = glyph(cell.dots);
            canvas.write({x, y}, std::u32string_view{&c, 1},
                         appearance({.background = cell.colors.background,
                                     .foreground = cell.colors.foreground}));
        }
    return {};
}
} // namespace ox
