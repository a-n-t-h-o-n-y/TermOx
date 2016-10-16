#include <mcurses/mcurses.hpp>

int main()
{
	mcurses::System system;

	mcurses::Widget widg(0, 0, mcurses::System::max_width, mcurses::System::max_height);

	system.set_head(&widg);

	return system.run();
}