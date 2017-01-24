#include <system_module/events/move_event.hpp>
#include <system_module/events/resize_event.hpp>
#include <widget_module/layouts/horizontal_layout.hpp>
#include <system_module/system.hpp>
#include <widget_module/border.hpp>
#include <widget_module/size_policy.hpp>
#include <painter_module/geometry.hpp>

#include <cstddef>

namespace twf {

void Horizontal_layout::update_geometry() {
    std::size_t border_space{0};
    std::size_t total_stretch{0};
    for (const Object* c : this->children()) {
        const Widget* cw{dynamic_cast<const Widget*>(c)};
        if (cw != nullptr) {
            // Border space
            if ((cw->border().west_enabled() ||
                 cw->border().north_west_enabled() ||
                 cw->border().south_west_enabled()) &&
                cw->border().enabled()) {
                ++border_space;
            }
            if ((cw->border().east_enabled() ||
                 cw->border().north_east_enabled() ||
                 cw->border().south_east_enabled()) &&
                cw->border().enabled()) {
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
            if ((cw->border().west_enabled() ||
                 cw->border().north_west_enabled() ||
                 cw->border().south_west_enabled()) &&
                cw->border().enabled()) {
                ++x_pos;
            }
            std::size_t y_pos{0};
            if ((cw->border().north_enabled() ||
                 cw->border().north_west_enabled() ||
                 cw->border().north_east_enabled()) &&
                cw->border().enabled()) {
                ++y_pos;
            }
            // Size
            std::size_t width =
                widg_space *
                (cw->size_policy().horizontal_stretch / double(total_stretch));
            std::size_t height{this->geometry().height()};
            if ((cw->border().north_enabled() ||
                 cw->border().north_west_enabled() ||
                 cw->border().north_east_enabled()) &&
                cw->border().enabled()) {
                --height;
            }
            if ((cw->border().south_enabled() ||
                 cw->border().south_west_enabled() ||
                 cw->border().south_east_enabled()) &&
                cw->border().enabled()) {
                --height;
            }
            System::post_event(cw,
                               std::make_unique<Resize_event>(width, height));
            System::post_event(cw, std::make_unique<Move_event>(
                                       this->x() + x_pos, this->y() + y_pos));
            x_pos += width;
            if ((cw->border().east_enabled() ||
                 cw->border().north_east_enabled() ||
                 cw->border().south_east_enabled()) &&
                cw->border().enabled()) {
                ++x_pos;
            }
        }
    }
}

}  // namespace twf
