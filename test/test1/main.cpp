#include <mcurses/mcurses.hpp>

class Click_paint_widget : public mcurses::Widget {
public:
	Click_paint_widget(unsigned x, unsigned y, unsigned width, unsigned height)
	:Widget(x,y,width,height)
	{}
private:

};

int main()
{
	mcurses::System system;

	Click_paint_widget widg(0, 0, mcurses::System::max_width, mcurses::System::max_height);

	system.set_head(&widg);

	return system.run();
}