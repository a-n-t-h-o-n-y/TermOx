#include <cppurses/system/detail/find_widget_at.hpp>

#include <cstddef>
#include <memory>

#include <cppurses/system/system.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace detail {

Widget* find_widget_at(std::size_t x, std::size_t y) {
    Widget* widg = System::head();
    if (widg == nullptr || !has_coordinates(*widg, x, y)) {
        return nullptr;
    }
    bool keep_going = true;
    while (keep_going && !widg->children.get().empty()) {
        for (const std::unique_ptr<Widget>& child : widg->children.get()) {
            if (has_coordinates(*child, x, y) && child->enabled()) {
                widg = child.get();
                keep_going = true;
                break;
            }
            keep_going = false;
        }
    }
    return widg;
}

}  // namespace detail
}  // namespace cppurses
