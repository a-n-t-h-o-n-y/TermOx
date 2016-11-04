#ifndef NCURSES_PAINT_ENGINE_HPP
#define NCURSES_PAINT_ENGINE_HPP

#include "../paint_engine.hpp"
#include "../color.hpp"
#include "../attribute.hpp"

#include <string>

namespace mcurses {
namespace detail {

class NCurses_paint_engine : public Paint_engine {
public:
	NCurses_paint_engine();
	virtual ~NCurses_paint_engine();
	virtual void refresh() override;
	virtual void move(unsigned x, unsigned y) override;
	virtual void put_char(char c) override;
	virtual void put_string(const std::string& s) override;
	virtual void show_cursor() override;
	virtual void hide_cursor() override;
	virtual unsigned screen_width() override;
	virtual unsigned screen_height() override;
	virtual void set_rgb(Color c, int r, int g, int b) override;
	virtual void set_attribute(Attribute attr) override;
	virtual void unset_attribute(Attribute attr) override;
	virtual void clear_attributes() override;
	virtual void set_background_color(Color c) override;
	virtual void set_foreground_color(Color c) override;

private:
	void initialize_color_pairs() const;
	int find_pair(Color foreground, Color background) const;
	static int translate_(Color c);
	static int attr_to_int(Attribute attr);
	Color current_foreground();
	Color current_background();
};

} // namespace detail
} // namespace mcurses

#endif // NCURSES_PAINT_ENGINE_HPP