#include <mcurses/painter_module/detail/ncurses_paint_engine.hpp>

#include <ncurses.h>

#include <string>

namespace mcurses {
namespace detail {

void
NCurses_paint_engine::move(unsigned x, unsigned y)
{
	::move(y, x);
	return;
}

void
NCurses_paint_engine::put_char(char c)
{
	::addch(c);
	return;
}

void
NCurses_paint_engine::put_string(const std::string& s)
{
	::addstr(s.c_str());
	return;
}

void
NCurses_paint_engine::show_cursor()
{
	::curs_set(1);
	return;
}
void
NCurses_paint_engine::hide_cursor()
{
	::curs_set(0);
	return;
}

} // namespace detail
} // namespace mcurses