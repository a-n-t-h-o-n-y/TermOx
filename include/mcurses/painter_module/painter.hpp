#ifndef PAINTER_HPP
#define PAINTER_HPP

#include "../widget_module/widget.hpp"
#include "color.hpp"

#include <string>
#include <sstream>
#include <codecvt>
#include <locale>


namespace mcurses {

class Painter {
public:
	Painter(Widget* widget):widget_{widget}
	{
		this->set_cursor(widget_->cursor());
		this->move(0,0);
	}
	template<typename T>
	void put(const T& item)
	{
		std::stringstream ss;
		ss << item;
		// Handle word wrapping if set and move internally held cursor coordinates
		// be aware of borders, should be a feature of widget

		// Number of characters in string, UTF8
		unsigned string_length = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(ss.str()).size();
		unsigned lines = (widget_->cursor_x() + string_length)/widget_->width() + 1;
		unsigned modulo = (widget_->cursor_x() + string_length)%widget_->width();

		// Set the position of the cursor after the print
		unsigned x = modulo + widget_->cursor_x();
		unsigned y = lines - 1 + widget_->cursor_y();

		// Print
		int last{0};
		for(int i{1}; i <= lines; ++i) {
			// Print a line and move the cursor
			this->put_to_engine_(ss.str().substr(last, widget_->width() - widget_->cursor_x()));
			last += widget_->width() - widget_->cursor_x();
			if(i != lines) {
				this->move_(0, widget_->cursor_y() + i);
			}
		}

		// Update internally held cursor position
		if(x >= widget_->width() || y >= widget_->height()) {
			widget_->set_cursor_x(widget_->width()-1);
			widget_->set_cursor_y(widget_->height()-1);
		} else {
			widget_->set_cursor_x(x);
			widget_->set_cursor_y(y);
		}
		return;
	}

	void refresh();
	void move(unsigned x, unsigned y);
	void fill(unsigned x, unsigned y, unsigned width, unsigned height, Color background);
	void set_cursor(bool state);
	void line(unsigned x1, unsigned y1, unsigned x2, unsigned y2, const std::string& b = "-");
	void border(const std::string& b);

	template<typename T>
	void put_feature_(const T& item)
	{
		std::stringstream ss;
		ss << item;
		this->put_to_engine_(ss.str());
		return;
	}

private:
	void move_(unsigned x, unsigned y);
	void put_to_engine_(const std::string& text);

	Widget* widget_;
	bool valid_position_ = true;
};

} // namespace mcurses

#endif // PAINTER_HPP