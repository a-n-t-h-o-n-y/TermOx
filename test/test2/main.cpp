#include <mcurses/mcurses.hpp>
using namespace mcurses;

class Textbox : public Widget {
public:
	Textbox() {
		this->set_focus_policy(mcurses::Widget::Focus_policy::StrongFocus);
		this->set_cursor(true);
		this->enable_border();
		this->brush().set_background(Color::Green);
	}

	virtual void key_press_event(Key_event& event) override {
		gs_.append(event.text(), background(Color::Red), foreground(Color::Blue));
		Widget::key_press_event(event);
		this->update();
	}

	virtual void paint_event(Paint_event& event) override {
		this->erase_widget_screen();
		Painter p{this};
		p.put(gs_);
	}

private:
	Glyph_string gs_;
};

class My_widg : public Horizontal_layout {
public:
	My_widg() {
		this->make_child<Textbox>();
		this->make_child<Textbox>();
	}
};

int main() {
	System system;

	My_widg main_widg;

	system.set_head(&main_widg);

	return system.run();
}