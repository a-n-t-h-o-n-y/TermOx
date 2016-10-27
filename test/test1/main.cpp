#include <mcurses/mcurses.hpp>

#include <sstream>

class Normal_widget : public mcurses::Widget {
public:
	Normal_widget()
	{
		this->set_show_cursor(false);
	}
	void paint_event(mcurses::Paint_event& event) override
	{
		mcurses::Painter p{this};
		p.border("-");
		p.move(0,0);
		p.put("Normal Widget");
		return;
	}
};

class Click_paint_widget : public mcurses::Widget {
public:
	Click_paint_widget()
	{
		this->set_show_cursor(false);
	}

	void paint_event(mcurses::Paint_event& event) override
	{
		mcurses::Painter p{this};
		p.border("*");
		p.move(0,0);
		p.put("Click Widget");
		return;
	}

	void mouse_press_event(mcurses::Mouse_event& event) override
	{
		mcurses::Painter p{this};
		p.move(event.local_x(), event.local_y());
		p.put("X");
		Widget::mouse_press_event(event);
	}
};

class Main_widget : public mcurses::Horizontal_layout {
public:
	Main_widget()
	{
		this->make_child<Click_paint_widget>();
		this->make_child<Normal_widget>();
		this->make_child<Click_paint_widget>();
	}
};

int main()
{
	mcurses::System system;

	Main_widget widg;

	system.set_head(&widg);

	return system.run();
}