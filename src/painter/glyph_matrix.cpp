#include <termox/painter/glyph_matrix.hpp>

#include <vector>

#include <termox/painter/glyph.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/point.hpp>

namespace ox {

Glyph_matrix::Glyph_matrix(Area area)
    : matrix_(area.height, std::vector<Glyph>(area.width, Glyph{U'\0'}))
{}

void Glyph_matrix::resize(Area area)
{
    matrix_.resize(area.height);
    matrix_.shrink_to_fit();
    for (auto& row : matrix_) {
        row.resize(area.width, U'\0');
        row.shrink_to_fit();
    }
}

void Glyph_matrix::clear() { matrix_.clear(); }

auto Glyph_matrix::width() const -> int
{
    return matrix_.empty() ? 0 : matrix_.at(0).size();
}

auto Glyph_matrix::height() const -> int { return matrix_.size(); }

auto Glyph_matrix::operator()(Point p) -> Glyph& { return matrix_[p.y][p.x]; }

auto Glyph_matrix::operator()(Point p) const -> Glyph
{
    return matrix_[p.y][p.x];
}

auto Glyph_matrix::at(Point p) -> Glyph& { return matrix_.at(p.y).at(p.x); }

auto Glyph_matrix::at(Point p) const -> Glyph
{
    return matrix_.at(p.y).at(p.x);
}

}  // namespace ox
