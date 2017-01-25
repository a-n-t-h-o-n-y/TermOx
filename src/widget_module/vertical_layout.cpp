#include <widget_module/layouts/vertical_layout.hpp>
#include <system_module/object.hpp>
#include <widget_module/widget.hpp>
#include <system_module/system.hpp>
#include <system_module/events/resize_event.hpp>
#include <system_module/events/move_event.hpp>

#include <cstddef>
#include <memory>

namespace twf {

void Vertical_layout::update_geometry() {
    std::size_t border_space{0};
    std::size_t total_stretch{0};
    for (const Object* c : this->children()) {
        const Widget* cw{dynamic_cast<const Widget*>(c)};
        if (cw != nullptr) {
            // Border space
            if ((cw->border().north_enabled() ||
                 cw->border().north_west_enabled() ||
                 cw->border().north_east_enabled()) &&
                cw->border().enabled()) {
                ++border_space;
            }
            if ((cw->border().south_enabled() ||
                 cw->border().south_west_enabled() ||
                 cw->border().south_east_enabled()) &&
                cw->border().enabled()) {
                ++border_space;
            }
            // Stretch factor
            total_stretch += cw->size_policy().vertical_stretch;
        }
    }
    std::size_t widg_space{0};
    if (this->geometry().height() < border_space) {
        widg_space = 0;
    } else {
        widg_space = this->geometry().height() - border_space;
    }

    std::size_t y_pos{0};
    for (Object* c : this->children()) {
        Widget* cw{dynamic_cast<Widget*>(c)};
        if (cw != nullptr) {
            // Position
            if ((cw->border().north_enabled() ||
                 cw->border().north_west_enabled() ||
                 cw->border().north_east_enabled()) &&
                cw->border().enabled()) {
                ++y_pos;
            }
            std::size_t x_pos{0};
            if ((cw->border().west_enabled() ||
                 cw->border().north_west_enabled() ||
                 cw->border().south_west_enabled()) &&
                cw->border().enabled()) {
                ++x_pos;
            }
            // Size
            if (total_stretch == 0) {
                total_stretch = this->children().size();
            }
            std::size_t height =
                widg_space *
                (cw->size_policy().vertical_stretch / double(total_stretch));
            std::size_t width{this->geometry().width()};
            if ((cw->border().west_enabled() ||
                 cw->border().north_west_enabled() ||
                 cw->border().south_west_enabled()) &&
                cw->border().enabled() && width != 0) {
                --width;
            }
            if ((cw->border().east_enabled() ||
                 cw->border().north_east_enabled() ||
                 cw->border().south_east_enabled()) &&
                cw->border().enabled() && width != 0) {
                --width;
            }
            System::post_event(cw,
                               std::make_unique<Resize_event>(width, height));
            System::post_event(cw, std::make_unique<Move_event>(
                                       this->x() + x_pos, this->y() + y_pos));
            y_pos += height;
            if ((cw->border().south_enabled() ||
                 cw->border().south_west_enabled() ||
                 cw->border().south_east_enabled()) &&
                cw->border().enabled()) {
                ++y_pos;
            }
            // if last widget, extend to rest of layout
            if (c == *(--std::end(this->children())) &&
                this->geometry().height() > y_pos) {
                System::post_event(
                    cw, std::make_unique<Resize_event>(
                            width, height + this->geometry().height() - y_pos));
            }
        }
    }
}

}  // namespace twf
