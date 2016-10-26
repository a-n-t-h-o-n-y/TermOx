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

unsigned NCurses_paint_engine::screen_width()
{
	int x{0}, y{0};
	getmaxyx(stdscr, y, x);
	return x;
}

unsigned NCurses_paint_engine::screen_height()
{
	int x{0}, y{0};
	getmaxyx(stdscr, y, x);
	return y;
}

void NCurses_paint_engine::fill_rect(unsigned x, unsigned y, unsigned width, unsigned height, Color background)
{
	// fill in to create a solid rectangle within the widget of background color.
}

} // namespace detail
} // namespace mcurses