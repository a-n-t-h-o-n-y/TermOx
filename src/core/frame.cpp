#include <ox/core/frame.hpp>

#include <algorithm>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>

namespace {
auto checked_coordinate(std::int64_t value) -> int
{
    if (value < std::numeric_limits<int>::min() ||
        value > std::numeric_limits<int>::max()) {
        throw std::overflow_error{"Coordinate overflow"};
    }
    return (int)value;
}

auto cell_count(ox::Area size) -> std::size_t
{
    ox::validate(size);
    auto const width = (std::size_t)size.width;
    auto const height = (std::size_t)size.height;
    auto const limit = std::vector<ox::Cell>{}.max_size();
    if (width && height > limit / width) {
        throw std::length_error{"Frame is too large"};
    }
    return width * height;
}
}  // namespace

namespace ox {
void validate(Style const& style)
{
    if (style.background.valueless_by_exception() ||
        style.foreground.valueless_by_exception()) {
        throw std::invalid_argument{"Frame requires a valid style"};
    }
}

void validate(Area size)
{
    if (size.width < 0 || size.height < 0) {
        throw std::invalid_argument{"Negative dimensions"};
    }
}

auto intersection(Rect one, Rect two) -> Rect
{
    validate(one.size);
    validate(two.size);
    auto const left = std::max(one.at.x, two.at.x);
    auto const top = std::max(one.at.y, two.at.y);
    auto const right = std::min((std::int64_t)one.at.x + one.size.width,
                                (std::int64_t)two.at.x + two.size.width);
    auto const bottom = std::min((std::int64_t)one.at.y + one.size.height,
                                 (std::int64_t)two.at.y + two.size.height);
    return {{left, top},
            {(int)std::max(std::int64_t{0}, right - left),
             (int)std::max(std::int64_t{0}, bottom - top)}};
}

auto contains(Rect rectangle, Point point) -> bool
{
    validate(rectangle.size);
    return point.x >= rectangle.at.x && point.y >= rectangle.at.y &&
           (std::int64_t)point.x <
               (std::int64_t)rectangle.at.x + rectangle.size.width &&
           (std::int64_t)point.y < (std::int64_t)rectangle.at.y + rectangle.size.height;
}

auto translate(Point point, Point offset) -> Point
{
    return {checked_coordinate((std::int64_t)point.x + offset.x),
            checked_coordinate((std::int64_t)point.y + offset.y)};
}

Frame::Frame(Area size, esc::WidthPolicy policy)
    : size_{size},
      cells_(cell_count(size)),
      revisions_((std::size_t)size.height),
      policy_{policy},
      overrides_{policy.overrides.begin(), policy.overrides.end()}
{
    (void)esc::cell_width(U"", policy);
    if (esc::grapheme_width(U" ", policy) != 1) {
        throw std::invalid_argument{"Frame blank space must occupy one column"};
    }
    policy_.overrides = {};
}

Frame::Frame(Frame const& other)
    : size_{other.size_},
      cells_{other.cells_},
      revisions_{other.revisions_},
      policy_{other.policy_},
      overrides_{other.overrides_}
{}

Frame::Frame(Frame&& other) noexcept
    : identity_{std::move(other.identity_)},
      size_{std::exchange(other.size_, Area{0, 0})},
      cells_{std::move(other.cells_)},
      revisions_{std::move(other.revisions_)},
      policy_{other.policy_},
      overrides_{std::move(other.overrides_)}
{}

auto Frame::operator=(Frame other) noexcept -> Frame&
{
    identity_.swap(other.identity_);
    revisions_.swap(other.revisions_);
    std::swap(size_, other.size_);
    cells_.swap(other.cells_);
    std::swap(policy_, other.policy_);
    overrides_.swap(other.overrides_);
    return *this;
}

auto Frame::width_policy() const -> esc::WidthPolicy
{
    auto policy = policy_;
    policy.overrides = overrides_;
    return policy;
}

auto Frame::index(Point point) const -> std::size_t
{
    if (!contains(Rect{{0, 0}, size_}, point)) {
        throw std::out_of_range{"Frame coordinate"};
    }
    return (std::size_t)point.y * (std::size_t)size_.width + (std::size_t)point.x;
}

auto Frame::operator[](Point point) const -> Cell const&
{ return cells_[this->index(point)]; }

auto Frame::row(int y) const -> std::span<Cell const>
{
    if (y < 0 || y >= size_.height) { throw std::out_of_range{"Frame row"}; }
    return std::span<Cell const>{cells_}.subspan(
        (std::size_t)y * (std::size_t)size_.width, (std::size_t)size_.width);
}

auto Frame::lead_at(Point point) const -> Point
{
    auto at = this->index(point);
    while (cells_[at].columns == 0) {
        --point.x;
        --at;
    }
    return point;
}

void Frame::resize(Area size)
{
    auto cells = std::vector<Cell>(cell_count(size));
    auto revisions = std::vector<std::uint64_t>((std::size_t)size.height);
    auto identity = std::shared_ptr<char const>{std::make_shared<char>()};
    cells_.swap(cells);
    revisions_.swap(revisions);
    identity_.swap(identity);
    size_ = size;
}

void Frame::changed(int row)
{
    auto& revision = revisions_[(std::size_t)row];
    if (revision == std::numeric_limits<std::uint64_t>::max()) {
        identity_ = std::make_shared<char>();
        std::ranges::fill(revisions_, 0);
    }
    ++revision;
}

void Frame::clear(Style style)
{
    validate(style);
    for (int y = 0; y < size_.height; ++y) {
        changed(y);
    }
    for (auto& cell : cells_) {
        if (cell.text.size() != 1 || cell.text[0] != U' ') {
            cell.text.assign(1, U' ');
        }
        cell.style = style;
        cell.columns = 1;
    }
}

auto Frame::write(Point at, std::u32string_view grapheme, Style style) -> bool
{
    validate(style);
    auto const width = esc::grapheme_width(grapheme, this->width_policy());
    if (!width || *width == 0) {
        throw std::invalid_argument{"Expected a printable grapheme"};
    }
    return write_validated(at, grapheme, style, Rect{{0, 0}, size_}, *width);
}

auto Frame::write_validated(Point at,
                            std::u32string_view grapheme,
                            Style style,
                            Rect clip,
                            std::size_t width) -> bool
{
    if (!contains(clip, at) ||
        width > (std::size_t)((std::int64_t)clip.at.x + clip.size.width - at.x)) {
        return false;
    }
    auto& target =
        cells_[(std::size_t)at.y * (std::size_t)size_.width + (std::size_t)at.x];
    // A one-column replacement cannot touch adjacent graphemes. Preserve the
    // general path for wide cells and their continuation columns.
    if (width == 1 && target.columns == 1) {
        auto const same_text = target.text == grapheme;
        if (same_text && target.style == style) return true;
        // Reuse the string for color-only changes; assign also permits text
        // borrowed from this cell, including a shorter substring.
        if (!same_text) target.text.assign(grapheme);
        target.style = style;
        changed(at.y);
        return true;
    }
    auto const last = Point{at.x + (int)width - 1, at.y};
    auto const first_lead = this->lead_at(at);
    auto const last_lead = this->lead_at(last);
    auto const end = last_lead.x + (int)(*this)[last_lead].columns;
    // Replacing either half clears the old grapheme, but only when its full footprint
    // is visible.
    if (first_lead.x < clip.at.x ||
        (std::int64_t)end > (std::int64_t)clip.at.x + clip.size.width) {
        return false;
    }
    auto const& existing = (*this)[at];
    if (existing.columns == width && existing.text == grapheme &&
        existing.style == style) {
        return true;
    }
    auto replacement = Cell{std::u32string{grapheme}, style, width};
    changed(at.y);
    for (int x = first_lead.x; x < end; ++x) {
        auto const index = this->index({x, at.y});
        cells_[index] = Cell{U" ", cells_[index].style, 1};
    }
    cells_[this->index(at)] = std::move(replacement);
    for (int x = at.x + 1; x <= last.x; ++x) {
        cells_[this->index({x, at.y})] = Cell{U"", style, 0};
    }
    return true;
}

auto Frame::restyle(Point at, Style style, Rect clip) -> bool
{
    validate(style);
    clip = intersection(clip, Rect{{0, 0}, size_});
    if (!contains(clip, at)) { return false; }
    auto const lead = this->lead_at(at);
    auto const end = lead.x + (int)(*this)[lead].columns;
    if (lead.x < clip.at.x ||
        (std::int64_t)end > (std::int64_t)clip.at.x + clip.size.width) {
        return false;
    }
    if ((*this)[lead].style == style) { return true; }
    changed(at.y);
    for (int x = lead.x; x < end; ++x) {
        cells_[this->index({x, at.y})].style = style;
    }
    return true;
}
}  // namespace ox
