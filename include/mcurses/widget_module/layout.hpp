#ifndef LAYOUT_HPP
#define LAYOUT_HPP

#include "../system_module/object.hpp"
#include "widget.hpp"

namespace mcurses {

// Base class for Layouts
class Layout : public Widget {
public:
	Layout(); // Takes main window coordinates

protected:

};

} // namespace mcurses
#endif // LAYOUT_HPP