#include <ox/align.hpp>

namespace ox {

auto find_align_offset(Align align, int cells_available, int cells_used) -> int
{
    switch (align) {
        case Align::Left: return 0;
        case Align::Center: return (cells_available - cells_used) / 2;
        case Align::Right: return cells_available - cells_used;
    }
    return 0;
}

}  // namespace ox