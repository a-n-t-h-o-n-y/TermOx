#include <system_module/events/move_event.hpp>
#include <system_module/events/resize_event.hpp>
#include <widget_module/layouts/horizontal_layout.hpp>

#include <cstddef>

namespace twf {

void Horizontal_layout::update_geometry() {
    // Number of children
    std::size_t n = this->children().size();
    // Height of each Widget
    std::size_t h = this->geometry().height();
    // Width of each Widget
    std::size_t w = this->geometry().width() / n; // this will be more complex


    std::size_t x{0};

    for (Object* c : this->children()) {
        Widget* child = dynamic_cast<Widget*>(c);
        if (child != nullptr && child->visible()) {
            System::post_event(child, std::make_unique<Resize_event>(w, h));
            System::post_event(child, std::make_unique<Move_event>(x, 0));
            x += w; // this should hopefully stay the same
        }
    }
}

}  // namespace twf


/* void Horizontal_layout::update_geometry() {
 *     std::size_t w = this->geometry.width();
 *     std::size_t h = this->geometry.height();
 * 
 *     std::size_t x{0};
 * 
 *     // find how many spaces will be taken by borders and shadows
 * 
 *     // use this space to find the width of each based on sf
 * 
 * 
 *     // find x position for top left of each child
 * 
 *     // find width w for each child
 * 
 *     // put the above found values on the children
 *     for(Object* c : this->children()) {
 *         Widget* child = dynamic_cast<Widget*>(c);
 *         if(child != nullptr && child->visible()) {
 *             System::post_event(child, std::make_unique<Resize_event>(w,h)); // the w will always change
 *             System::post_event(child, std::make_unique<Move_event>(x, 0));  // the x will always change
 *         }
 *     }
 * } */
