#include <mcurses/painter_module/detail/ncurses_paint_engine.hpp>
#include <mcurses/painter_module/color.hpp>
#include <mcurses/painter_module/attribute.hpp>

#include <ncurses.h>

#include <string>
#include <vector>

namespace mcurses {
namespace detail {

void
NCurses_paint_engine::refresh()
{
	::wrefresh(::stdscr);
	return;
}

void
NCurses_paint_engine::move(unsigned x, unsigned y)
{
	::wmove(::stdscr, y, x);
	return;
}

void
NCurses_paint_engine::put_char(char c)
{
	::waddch(::stdscr, c);
	return;
}

void
NCurses_paint_engine::put_string(const std::string& s)
{
	::waddstr(::stdscr, s.c_str());
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

void
NCurses_paint_engine::set_rgb(Color c, int r, int g, int b) {
	auto scale = [](int i){ return (i/255) * 1000; };
	int r_ = scale(r);
	int g_ = scale(g);
	int b_ = scale(b);
	::init_color(translate_(c), r_, g_, b_);
}

int
NCurses_paint_engine::attr_to_int(Attribute attr) {
	int a = A_NORMAL;
	switch(attr) {
		case Attribute::Bold :
			a = A_BOLD;
			break;

		case Attribute::Italic :
			a = A_ITALIC;
			break;

		case Attribute::Underline :
			a = A_UNDERLINE;
			break;

		case Attribute::Standout :
			a = A_STANDOUT;
			break;

		case Attribute::Dim :
			a = A_DIM;
			break;

		case Attribute::Inverse :
			a = A_REVERSE;
			break;

		case Attribute::Invisible :
			a = A_INVIS;
			break;

		case Attribute::Blink :
			a = A_BLINK;
			break;
	}
	return a;
}

void
NCurses_paint_engine::set_attribute(Attribute attr) {
	::wattron(::stdscr, attr_to_int(attr));
	return;
}

void
NCurses_paint_engine::unset_attribute(Attribute attr) {
	::wattroff(::stdscr, attr_to_int(attr));
	return;
}

void
NCurses_paint_engine::clear_attributes() {
	::wattron(::stdscr, A_NORMAL);
	return;
}

void
NCurses_paint_engine::set_background_color(Color c) {
	::wattron(::stdscr, COLOR_PAIR(this->find_pair(this->current_foreground(), c)));
	return;
}

void
NCurses_paint_engine::set_foreground_color(Color c) {
	::wattron(::stdscr, COLOR_PAIR(this->find_pair(c, this->current_background())));
	return;
}

Color NCurses_paint_engine::current_foreground() {
	int attrs;
	int pair;
	wattr_get(::stdscr, &attrs, &pair, nullptr);
	return static_cast<Color>(pair/16 + 240);
}

Color NCurses_paint_engine::current_background() {
	int attrs;
	int pair;
	wattr_get(::stdscr, &attrs, &pair, nullptr);
	return static_cast<Color>(pair%16 + 240);
}

void
NCurses_paint_engine::initialize_color_pairs() const {
	std::vector<int> int_colors = {240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255};
	int index{0};
	for(int i{0}; i<16; ++i) {
		for(int j{0}; j<16; ++j) {
			::init_pair(index++, int_colors[i], int_colors[j]);
		}
	}
}

int
NCurses_paint_engine::find_pair(Color foreground, Color background) const {
	return (this->translate_(foreground) - 240)*16 + (this->translate_(background)-240);
}

int
NCurses_paint_engine::translate_(Color c) {
	return static_cast<int>(c) + 240;
}


} // namespace detail
} // namespace mcurses