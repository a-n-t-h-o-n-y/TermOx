#include <mcurses/mcurses.hpp>

#include <string>

class Text_box : public mcurses::Widget {
public:
	Text_box()
	{
		this->set_focus_policy(mcurses::Widget::Focus_policy::StrongFocus);
		this->set_cursor(true);
	}
	void virtual paint_event(mcurses::Paint_event& event) override
	{
		this->erase_widget_screen();
		mcurses::Painter p{this};
		p.move(0,0);
		p.put(contents_);
		Widget::paint_event(event);
		return;
	}
	void virtual key_press_event(mcurses::Key_event& event) override
	{
		contents_.append(event.text());
		Widget::key_press_event(event);
		this->update();
		return;
	}
private:
	std::string contents_;
};

class Normal_widget : public mcurses::Widget {
public:
	Normal_widget()
	{
		this->set_focus_policy(mcurses::Widget::Focus_policy::StrongFocus);
		this->set_cursor(true);
		this->enable_border();
	}
	void paint_event(mcurses::Paint_event& event) override
	{
		this->erase_widget_screen();
		mcurses::Painter p{this};
		p.move(1,1);
		p.put("Normal Widget");
		p.move(this->geometry().width()/2,this->geometry().height()/2);
		p.put(text_);
		Widget::paint_event(event);
		return;
	}
	void key_press_event(mcurses::Key_event& event) override
	{
		text_ = event.text();
		this->update();
	}
private:
	std::string text_; // change to glyph
};

class Click_paint_widget : public mcurses::Widget {
public:
	Click_paint_widget()
	{
		this->set_cursor(false);
		this->enable_border();
	}

	void paint_event(mcurses::Paint_event& event) override
	{
		mcurses::Painter p{this};
		p.move(1,1);
		p.put("Click Widget");
		Widget::paint_event(event);
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
		this->make_child<Text_box>();
	}
};

int main()
{
	mcurses::System system;

	Main_widget widg;

	system.set_head(&widg);

	return system.run();
}