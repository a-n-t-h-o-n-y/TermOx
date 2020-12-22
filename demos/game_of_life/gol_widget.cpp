#include "gol_widget.hpp"

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstddef>
#include <iterator>
#include <string>
#include <vector>

#include <termox/painter/glyph.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

#include "cell.hpp"
#include "colors.hpp"
#include "coordinate.hpp"
#include "exporters.hpp"
#include "filetype.hpp"
#include "get_life_1_05.hpp"
#include "get_life_1_06.hpp"
#include "get_plaintext.hpp"
#include "get_rle.hpp"
#include "pattern.hpp"

namespace {
using gol::Coordinate;

/// Applies the given \p offset to each cell Coordinate in \p cells.
template <typename Container_t>
void apply(bool hi_res, Coordinate offset, Container_t& cells)
{
    if (hi_res) {
        offset.x *= 2;
        offset.y *= 4;
    }
    std::transform(std::begin(cells), std::end(cells), std::begin(cells),
                   [offset](Coordinate cell) -> Coordinate {
                       return {cell.x + offset.x, cell.y + offset.y};
                   });
}

}  // namespace

using namespace ox;
namespace gol {

void GoL_widget::import_file(std::string const& filename)
{
    auto pattern = Pattern{};
    switch (get_filetype(filename)) {
        case FileType::Life_1_05: pattern = get_life_1_05(filename); break;
        case FileType::Life_1_06: pattern = get_life_1_06(filename); break;
        case FileType::Plaintext: pattern = get_plaintext(filename); break;
        case FileType::RLE: pattern = get_RLE(filename); break;
        case FileType::Unknown: return;
    }
    this->import_pattern(pattern);
}

void GoL_widget::import_pattern(Pattern pattern)
{
    // This centers the cells to the current screen display
    apply(hi_res_, offset_, pattern.cells);
    this->set_pattern(pattern);
}

void GoL_widget::export_to_file(std::string const& filename)
{
    // TODO apply offset backwards, negative? but you have to do it in the
    // files.. you could just pass on a container or something?
    const auto ext = get_extension(filename);
    if (ext == "lif")
        export_as_life_1_05(filename, engine_);
    else if (ext == "life")
        export_as_life_1_06(filename, engine_);
    else if (ext == "cell")
        export_as_plaintext(filename, engine_);
    else if (ext == "rle")
        export_as_rle(filename, engine_);
}

}  // namespace gol
