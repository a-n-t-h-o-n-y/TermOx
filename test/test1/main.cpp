#include <mcurses/mcurses.hpp>

#include <sstream>

class Click_paint_widget : public mcurses::Widget {
public:
	Click_paint_widget(unsigned x, unsigned y, unsigned width, unsigned height)
	{
		this->set_geometry(x,y,width,height);
		this->set_show_cursor(false);
	}

	void paint_event(mcurses::Paint_event& event) override
	{
		// mcurses::Painter p;
		// p.touch(event->region());
		// return;
	}

	void mouse_press_event(mcurses::Mouse_event& event) override
	{
		mcurses::Painter p{this};
		p.move(event.local_x(), event.local_y());
		// std::stringstream ss;
		// ss << "x:" << mcurses::System::max_width() << ",y:" << mcurses::System::max_height();
		// p.put(ss.str());
		p.put("Ѩ҉ξᾨ");
		Widget::mouse_press_event(event);
	}
};

int main()
{
	mcurses::System system;

	Click_paint_widget widg(0, 0, mcurses::System::max_width()/2, mcurses::System::max_height()/2);

	system.set_head(&widg);

	return system.run();
}