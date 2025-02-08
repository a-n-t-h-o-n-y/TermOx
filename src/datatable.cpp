#include <ox/datatable.hpp>

#include <ox/align.hpp>
#include <ox/core/core.hpp>

#include <algorithm>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace ox {

DataTable::Options const DataTable::init = {};

DataTable::DataTable(Options x)
    : Widget{FocusPolicy::None, SizePolicy::flex()},
      cell_brush{std::move(x.cell_brush)},
      line_brush{std::move(x.line_brush)},
      background{std::move(x.background)}
{}

void DataTable::add_column(std::string heading, Align align, Color foreground)
{
    headings_.children.push_back({{
        .text = std::move(heading),
        .align = align,
        .brush = {.foreground = foreground, .traits = Trait::Bold},
    }});
    auto const existing_size = columns_.empty() ? 0 : columns_.front().size();
    columns_.push_back(std::vector<std::string>(existing_size, ""));
}

void DataTable::add_row(std::vector<std::string> row)
{
    if (row.size() != columns_.size()) {
        throw std::runtime_error{
            "DataTable::add_row: row size does not match column count."};
    }
    for (auto i = std::size_t{0}; i < columns_.size(); ++i) {
        columns_[i].push_back(std::move(row[i]));
    }
}

void DataTable::paint(Canvas c)
{
    if (this->size.width == 0 || this->size.height == 0 || columns_.empty()) {
        return;
    }

    auto const border_area = Area{
        .width = c.size.width,
        .height = std::min(c.size.height,
                           ((int)columns_.front().size() - (int)this->offset) * 2 + 3),
    };

    auto painter = Painter{c};

    // Border
    painter[{0, 0}] << Painter::Box{
        .corners = Painter::Box::round_corners,
        .brush = line_brush,
        .size = border_area,
    };

    // Header Line
    if (this->size.height > 2) {
        painter[{0, 2}]
            << (U'╞' | line_brush)
            << Painter::HLine{
                .length = std::max(c.size.width - 2, 0),
                .glyph = {.symbol = U'═', .brush = line_brush},
            }
            << (U'╡' | line_brush);
    }
    auto const table_y = 3;

    // Cell Text
    for (std::size_t column_index = 0; column_index < columns_.size(); ++column_index) {
        auto const& column = columns_[column_index];

        auto cell_point = Point{
            .x = headings_.children[column_index].at.x + 1,
            .y = table_y,
        };
        auto const cell_width = headings_.children[column_index].size.width;

        for (auto const& text : std::span{column}.subspan(this->offset)) {
            if (cell_point.y + 1 >= border_area.height) {
                break;
            }
            if (text.size() + 1 > (std::size_t)cell_width) {
                painter[cell_point]
                    << text.substr(0, (std::size_t)std::max(cell_width - 2, 0)) + "…";
            }
            else {
                painter[cell_point] << text;
            }
            cell_point.y += 2;
        }
    }

    // Border Intersections
    for (auto y = 4; y + 1 < border_area.height; y += 2) {
        painter[{0, y}] << (U'├' | line_brush);
        painter[{c.size.width - 1, y}] << (U'┤' | line_brush);
    }
    for (auto i = std::size_t{0}; i + 1 < headings_.children.size(); ++i) {
        auto const& heading = headings_.children[i];
        auto const x = heading.at.x + heading.size.width;
        painter[{x, 2}] << (U'╤' | line_brush);
        painter[{x, std::max(border_area.height - 1, 0)}] << (U'┴' | line_brush);
    }

    // Horizontal Lines
    for (auto y = 4; y + 1 < border_area.height; y += 2) {
        for (auto column_index = std::size_t{0}; column_index < columns_.size();
             ++column_index) {
            auto const& heading = headings_.children[column_index];
            auto x = heading.at.x;
            painter[{x + 1, y}] << Painter::HLine{
                .length = heading.size.width,
                .glyph = {.symbol = U'─', .brush = line_brush},
            };
        }
    }

    // Vertical Lines and Intersection
    for (auto y = 3; y + 1 < border_area.height; y += 2) {
        for (auto column_index = std::size_t{0}; column_index + 1 < columns_.size();
             ++column_index) {
            auto const& heading = headings_.children[column_index];
            auto x = heading.at.x + heading.size.width;
            painter[{x, y}] << (U'│' | line_brush);
            if (y + 2 < border_area.height) {
                painter[{x, y + 1}] << (U'┼' | line_brush);
            }
        }
    }

    for (auto x = 0; x < c.size.width; ++x) {
        for (auto y = 0; y < c.size.height; ++y) {
            c[{x, y}].brush.background = this->background;
        }
    }
}

void DataTable::mouse_wheel(Mouse m)
{
    if (m.button == Mouse::Button::ScrollUp) {
        offset = (std::size_t)std::max(0, (int)offset - 1);
    }
    else if (m.button == Mouse::Button::ScrollDown) {
        offset = (std::size_t)std::min(
            columns_.empty() ? 0 : (int)columns_.front().size() - 1, (int)offset + 1);
    }
}

void DataTable::resize(Area)
{
    auto const old_size = headings_.size;
    headings_.at = {.x = 1, .y = 1};
    headings_.size = {.width = size.width - 2, .height = 1};
    headings_.resize(old_size);
}

auto DataTable::get_children() -> Generator<Widget&> { co_yield headings_; }

auto DataTable::get_children() const -> Generator<Widget const&> { co_yield headings_; }

}  // namespace ox