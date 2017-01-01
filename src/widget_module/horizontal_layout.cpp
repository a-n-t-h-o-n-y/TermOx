#include <system_module/events/move_event.hpp>
#include <system_module/events/resize_event.hpp>
#include <widget_module/layouts/horizontal_layout.hpp>

#include <cstddef>

namespace twf {

void Horizontal_layout::update_geometry() {
    std::size_t n = this->children().size();
    std::size_t h = this->geometry().height();
    std::size_t w = this->geometry().width() / n;
    std::size_t x{0};
    std::size_t y{0};

    for (Object* c : this->children()) {
        Widget* child = dynamic_cast<Widget*>(c);
        if (child != nullptr && child->visible()) {
            System::post_event(child, std::make_unique<Resize_event>(w, h));
            // this is fine, it is not global^^
            System::post_event(child, std::make_unique<Move_event>(x, y));
            // are you posting global coords to local spots?? ^^
            x += w;
        }
    }
}

}  // namespace twf
