#include <mcurses/painter_module/painter.hpp>
#include <mcurses/painter_module/color.hpp>
#include <mcurses/painter_module/glyph_string.hpp>
#include <mcurses/painter_module/attribute.hpp>
#include <mcurses/painter_module/geometry.hpp>
#include <mcurses/widget_module/border.hpp>

namespace mcurses {

Painter::Painter(Widget* widget):widget_{widget} {
	this->set_cursor(widget_->cursor());
	this->move(0,0);
}

// void
// Painter::put(const Glyph_string& gs) {
// 	this->move(widget_->cursor_x(), widget_->cursor_y());
// 	for(const Glyph& g : gs) {
// 		this->put_glyph_(g);
// 		this->move(widget_->cursor_x() + 1, widget_->cursor_y());
// 	}
// }

void
Painter::put(const Glyph_string& gs) {
	for(Glyph g : gs) {
		if(!g.brush().background_color()) {
			g.brush().add_attributes(background(*widget_->brush().background_color()));
		}
		if(!g.brush().foreground_color()) {
			g.brush().add_attributes(foreground(*widget_->brush().background_color()));
		}
		for(Attribute& attr : widget_->brush().attributes()) {
			g.brush().add_attributes(attr);
		}

		unsigned glob_x = widget_->global_x() + widget_->cursor_x();
		unsigned glob_y = widget_->global_y() + widget_->cursor_y();
		widget_->paint_engine().put(glob_x ,glob_y , g);
		this->move(widget_->cursor_x() + 1, widget_->cursor_y());
	}
}

void Painter::move(unsigned x, unsigned y) {
	widget_->set_cursor_x(x);
	widget_->set_cursor_y(y);

	// If out of widget_'s bounds
	Geometry& geo = widget_->geometry();
	if(x < geo.active_x_min()) { this->move(geo.active_x_min(), y); return; }
	if(x > geo.active_x_max()) { this->move(geo.active_x_min(), y+1); return; }
	if(y < geo.active_y_min()) { this->move(x, geo.active_y_min()); return; }
	if(y > geo.active_y_max()) { this->move(x, geo.active_y_max()); return; }

	// unsigned glob_x = widget_->global_x() + x;
	// unsigned glob_y = widget_->global_y() + y;

	// widget_->paint_engine().move(glob_x, glob_y);
	return;
}

// void
// Painter::refresh() // should not need this function
// {
// 	widget_->paint_engine().refresh();
// }

// Does not move the widget's internal cursor position
// void
// Painter::move_(unsigned x, unsigned y)
// {
// 	Geometry& geo = widget_->geometry();
// 	if(x > geo.width()-1) { this->move_(0, y+1); return; }
// 	if(y > geo.height()-1) { this->move_(x, widget_->geometry().height()-1); return; }

// 	unsigned glob_x = widget_->global_x() + x;
// 	unsigned glob_y = widget_->global_y() + y;

// 	widget_->paint_engine().move(glob_x, glob_y);
// 	return;
// }

// void
// Painter::put_glyph_(const Glyph& g) {
// 	// Set Widget's defaults
// 	for(const Attribute& a : widget_->brush().attributes()) {
// 		widget_->paint_engine().set_attribute(a);
// 	}
// 	// if(widget_->brush().background_color()) {
// 	// 	widget_->paint_engine().set_background_color(*widget_->brush().background_color());
// 	// }
// 	// if(widget_->brush().foreground_color()) {
// 	// 	widget_->paint_engine().set_foreground_color(*widget_->brush().foreground_color());
// 	// }

// 	// Set Colors
// 	if(g.brush().background_color()) {
// 		widget_->paint_engine().set_background_color(*g.brush().background_color());
// 	} else {
// 		widget_->paint_engine().set_background_color(*widget_->brush().background_color());
// 	}

// 	if(g.brush().foreground_color()) {
// 		widget_->paint_engine().set_foreground_color(*g.brush().foreground_color());
// 	} else {
// 		widget_->paint_engine().set_foreground_color(*widget_->brush().foreground_color());
// 	}

// 	// Set Attributes
// 	for(const Attribute& a : g.brush().attributes()) {
// 		widget_->paint_engine().set_attribute(a);
// 	}

// 	// Put Character
// 	widget_->paint_engine().put_string(g.str());

// 	// Reset Attributes
// 	widget_->paint_engine().clear_attributes();
// }

// Does not move widget held cursor position
// void
// Painter::put_(const Glyph_string& gs)
// {
// 	for(const Glyph& g : gs) {
// 		this->put_glyph_(g);
// 		this->move_(widget_->cursor_x() + 1, widget_->cursor_y());
// 	}
// 	return;
// }

void Painter::fill(unsigned x, unsigned y, unsigned width, unsigned height, Color fill_background) {
	// Save current cursor position
	unsigned cur_x = widget_->cursor_x();
	unsigned cur_y = widget_->cursor_y();

	Glyph_string tile{" ", background(fill_background)};
	for(unsigned i{y}; i < height; ++i) {
		this->line(x, i, width-1, i, tile);
	}

	// Set Widget's cursor position back
	widget_->set_cursor_x(cur_x);
	widget_->set_cursor_y(cur_y);

	return;
}

void Painter::line(unsigned x1, unsigned y1, unsigned x2, unsigned y2, const Glyph_string& gs) {
	// Save current cursor position
	unsigned cur_x = widget_->cursor_x();
	unsigned cur_y = widget_->cursor_y();

	// points must be left to right/ top to bottom right now, implement alternatives.
	// Horizontal
	if(y1 == y2) {
		for(unsigned i{x1}; i <= x2; ++i) {
			this->move(i, y1);
			this->put(gs);
		}
	}
	// Vertical
	else if(x1 == x2) {
		for(unsigned i{y1}; i <= y2; ++i) {
			this->move(x1, i);
			this->put(gs);
		}
	}
	// Diagonal not implemented right now

	// Set Widget's cursor position back
	widget_->set_cursor_x(cur_x);
	widget_->set_cursor_y(cur_y);

	return;
}

void Painter::border(const Border& b) {
	// Save current cursor position
	unsigned cur_x = widget_->cursor_x();
	unsigned cur_y = widget_->cursor_y();

	// North
	this->line(1, 0, widget_->geometry().width()-2, 0, b.north());

	// South
	this->line(1, widget_->geometry().height()-1, widget_->geometry().width()-2, widget_->geometry().height()-1, b.south());

	// East
	this->line(widget_->geometry().width()-1, 1, widget_->geometry().width()-1, widget_->geometry().height()-2, b.east());

	// West
	this->line(0, 1, 0, widget_->geometry().height()-2, b.west());	

	// North - West
	this->move(0, 0);
	this->put(b.north_west());

	// North - East
	this->move(widget_->geometry().width()-1, 0);
	this->put(b.north_east());

	// South - West
	this->move(0, widget_->geometry().height()-1);
	this->put(b.south_west());

	// South - East
	this->move(widget_->geometry().width()-1, widget_->geometry().height()-1);
	this->put(b.south_east());

	// Set Widget's cursor position back
	widget_->set_cursor_x(cur_x);
	widget_->set_cursor_y(cur_y);
}

void Painter::border(const Glyph_string& gs){ // get rid of this...? {
	// Save current cursor position
	unsigned cur_x = widget_->cursor_x();
	unsigned cur_y = widget_->cursor_y();

	// Top border
	this->line(0, 0, widget_->geometry().width()-1, 0, gs);

	// Bottom border
	this->line(0, widget_->geometry().height()-1, widget_->geometry().width()-1, widget_->geometry().height()-1, gs);

	// Middle sides
	this->line(0, 0, 0, widget_->geometry().height()-1, gs);
	this->line(widget_->geometry().width()-1, 0, widget_->geometry().width()-1, widget_->geometry().height()-1, gs);

	// Set Widget's cursor position back
	widget_->set_cursor_x(cur_x);
	widget_->set_cursor_y(cur_y);
}

void
Painter::set_cursor(bool state)
{
	if(state) {
		widget_->paint_engine().show_cursor();
	} else {
		widget_->paint_engine().hide_cursor();
	}
	return;
}

} // namespace mcurses