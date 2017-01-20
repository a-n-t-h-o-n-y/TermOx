#include <system_module/events/move_event.hpp>
#include <system_module/events/resize_event.hpp>
#include <widget_module/layouts/horizontal_layout.hpp>
#include <system_module/system.hpp>
#include <widget_module/border.hpp>
#include <widget_module/size_policy.hpp>
#include <painter_module/geometry.hpp>

#include <cstddef>

namespace twf {

/* void Horizontal_layout::update_geometry() {
 *     // Number of children
 *     std::size_t n = this->children().size();
 *     // Height of each Widget
 *     std::size_t h = this->geometry().height();
 *     // Width of each Widget
 *     std::size_t w = this->geometry().width() / n; // this will be more
 * complex
 *
 *
 *     std::size_t x{0};
 *
 *     for (Object* c : this->children()) {
 *         Widget* child = dynamic_cast<Widget*>(c);
 *         if (child != nullptr && child->visible()) {
 *             System::post_event(child, std::make_unique<Resize_event>(w, h));
 *             System::post_event(child, std::make_unique<Move_event>(x, 0));
 *             x += w; // this should hopefully stay the same
 *         }
 *     }
 * } */
void Horizontal_layout::update_geometry() {
    std::size_t border_space{0};
    std::size_t total_stretch{0};
    for (const Object* c : this->children()) {
        const Widget* cw{dynamic_cast<const Widget*>(c)};
        if (cw != nullptr) {
            // Border space
            if (cw->border().west_enabled() && cw->border().enabled()) {
                ++border_space;
            }
            if (cw->border().east_enabled() && cw->border().enabled()) {
                ++border_space;
            }
            // Stretch factor
            total_stretch += cw->size_policy().horizontal_stretch;
        }
    }
    std::size_t widg_space = this->geometry().width() - border_space;
    std::size_t x_pos{0};
    for (Object* c : this->children()) {
        Widget* cw{dynamic_cast<Widget*>(c)};
        if (cw != nullptr) {
            // Position
            if (cw->border().west_enabled() && cw->border().enabled()) {
                ++x_pos;
            }
            std::size_t y_pos{0};
            if (cw->border().north_enabled() && cw->border().enabled()) {
                ++y_pos;
            }
            // Size
            std::size_t width = 
                widg_space *
                (cw->size_policy().horizontal_stretch / double(total_stretch));
            std::size_t height{this->geometry().height()};
            if (cw->border().north_enabled() && cw->border().enabled()) {
                --height;
            }
            if (cw->border().south_enabled() && cw->border().enabled()) {
                --height;
            }
            System::post_event(cw,
                               std::make_unique<Resize_event>(width, height));
            System::post_event(cw, std::make_unique<Move_event>(
                                       this->x() + x_pos, this->y() + y_pos));
            x_pos += width;
            if(cw->border().east_enabled() && cw->border().enabled()) {
                ++x_pos;
            }
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
 *             System::post_event(child, std::make_unique<Resize_event>(w,h));
 * // the w will always change
 *             System::post_event(child, std::make_unique<Move_event>(x, 0));
 * // the x will always change
 *         }
 *     }
 * } */
