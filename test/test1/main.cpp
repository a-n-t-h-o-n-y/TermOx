#include <mcurses/mcurses.hpp>

class Click_paint_widget : public mcurses::Widget {
public:
	Click_paint_widget(unsigned x, unsigned y, unsigned width, unsigned height)
	:Widget(x,y,width,height)
	{
		show_cursor_ = false;
	}

	void repaint_event(const mcurses::Paint_event& event)
	{
		// mcurses::Painter p;
		// p.touch(event->region());
		// return;
	}

	void mouse_press_event(const mcurses::Mouse_event& event)
	{
		// mcurses::System::exit();
		mcurses::Painter p{this};
		p.move(event.local_x(), event.local_y());
		// std::stringstream ss;
		// ss << "x:" << mcurses::System::max_width() << ",y:" << mcurses::System::max_height();
		// p.put(ss.str());
		p.put("Ѩ҉ξᾨ");
	}

	bool event(const mcurses::Event& event) override
	{
		if(event.type() == mcurses::Event::Type::MouseButtonPress)
		{
			this->mouse_press_event(static_cast<const mcurses::Mouse_event&>(event));
		}
		if(event.type() == mcurses::Event::Type::Paint)
		{
			this->repaint_event(static_cast<const mcurses::Paint_event&>(event));
		}
		return true;
	}
private:

};

int main()
{
	mcurses::System system;

	Click_paint_widget widg(0, 0, mcurses::System::max_width, mcurses::System::max_height);

	system.set_head(&widg);

	return system.run();
}