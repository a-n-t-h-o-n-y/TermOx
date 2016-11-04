#include <mcurses/widget_module/layouts/horizontal_layout.hpp>
#include <mcurses/system_module/events/resize_event.hpp>
#include <mcurses/system_module/events/move_event.hpp>

namespace mcurses {

void Horizontal_layout::update_geometry()
{
	unsigned n = this->children().size();
	unsigned h = this->geometry().height();
	unsigned w = this->geometry().width()/ n;
	unsigned x{0};
	unsigned y{0};

	for(Object* c : this->children()) {
		Widget* child = dynamic_cast<Widget*>(c);
		if(child && child->visible()) {
			System::post_event(child, std::make_unique<Resize_event>(w, h)); // this is fine, it is not global
			System::post_event(child, std::make_unique<Move_event>(x, y)); // are you posting global coords to local spots??
			x += w;
		}
	}
}

} // namespace mcurses