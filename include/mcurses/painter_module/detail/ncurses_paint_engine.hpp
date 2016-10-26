#ifndef NCURSES_PAINT_ENGINE_HPP
#define NCURSES_PAINT_ENGINE_HPP

#include "../paint_engine.hpp"
#include "../color.hpp"

#include <string>

namespace mcurses {
namespace detail {

class NCurses_paint_engine : public Paint_engine {
public:
	virtual void move(unsigned x, unsigned y) override;
	virtual void put_char(char c) override;
	virtual void put_string(const std::string& s) override;
	virtual void show_cursor() override;
	virtual void hide_cursor() override;
	virtual unsigned screen_width() override;
	virtual unsigned screen_height() override;
	virtual void fill_rect(unsigned x, unsigned y, unsigned width, unsigned height, Color background) override;

private:

};

} // namespace detail
} // namespace mcurses

#endif // NCURSES_PAINT_ENGINE_HPP