#include <cppurses/system/events/delete_event.hpp>

#include <vector>

#include <cppurses/widget/widget.hpp>

namespace cppurses {

bool Delete_event::send() const
{
    const auto result = receiver_.delete_event();
    if (removed_ == nullptr)
        return result;
    for (Widget* w : removed_->get_descendants()) {
        w->delete_event();
    }
    removed_.reset();
    return result;
}

}  // namespace cppurses
