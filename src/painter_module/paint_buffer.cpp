#include <painter_module/detail/paint_buffer.hpp>

namespace mcurses {
namespace detail {

bool
Paint_buffer::commit(unsigned x, unsigned y) {
	if(staging_area_.at(x,y) == backing_store_.at(x,y)) {
		return false;
	}
	backing_store_.at(x,y) = staging_area_.at(x,y);
	return true;
}

} // namespace detail
} // namespace mcurses
