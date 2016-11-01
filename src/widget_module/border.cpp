#include <mcurses/widget_module/border.hpp>
#include <mcurses/painter_module/painter.hpp>
#include <mcurses/painter_module/glyph.hpp>

namespace mcurses {

void Border::draw() const {
	Painter p{widget_};
	// North
	p.line(1, 0, widget_->width()-2, 0, north_.str());

	// South
	p.line(1, widget_->height()-1, widget_->width()-2, widget_->height()-1, south_.str());

	// East
	p.line(widget_->width()-1, 1, widget_->width()-1, widget_->height()-2, east_.str());

	// West
	p.line(0, 1, 0, widget_->height()-2, west_.str());	

	// North - West
	p.move(0, 0);
	p.put_feature_(north_west_.str());

	// North - East
	p.move(widget_->width()-1, 0);
	p.put_feature_(north_east_.str());

	// South - West
	p.move(0, widget_->height()-1);
	p.put_feature_(south_west_.str());

	// South - East
	p.move(widget_->width()-1, widget_->height()-1);
	p.put_feature_(south_east_.str());

	return;
}

void Border::set_walls(const Glyph& horizontals, const Glyph& verticals)
{
	north_ = horizontals;
	south_ = horizontals;
	east_ = verticals;
	west_ = verticals;
	return;
}

} // namespace mcurses