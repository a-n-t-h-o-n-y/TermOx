#include <ox/widgets/focus.hpp>

#include <ox/widgets/widget.hpp>

namespace ox {

void Focus::set(Widget& w)
{
    if (in_focus_ != nullptr) {
        focus_out(*in_focus_);
    }
    in_focus_ = &w;
    if (in_focus_ != nullptr) {
        focus_in(w);
    }
}

void Focus::clear()
{
    if (in_focus_ != nullptr) {
        focus_out(*in_focus_);
    }
    in_focus_ = nullptr;
}

}  // namespace ox