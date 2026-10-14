#include <ox/align.hpp>

#include <stdexcept>

namespace ox {

auto align_offset(Align align, int cells_available, int cells_used) -> int
{
    if (cells_available < 0 || cells_used < 0) {
        throw std::invalid_argument{"Negative alignment extent"};
    }
    switch (align) {
        case Align::Start: return 0;
        case Align::Center: return (cells_available - cells_used) / 2;
        case Align::End: return cells_available - cells_used;
    }
    throw std::invalid_argument{"Invalid alignment"};
}

}  // namespace ox