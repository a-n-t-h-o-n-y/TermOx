#include <ox/widgets/focus.hpp>

#include <ox/widgets/widget.hpp>

namespace ox {

void Focus::set(Widget& w)
{
    if (in_focus_.valid()) {
        in_focus_.get().focus_out();
    }
    in_focus_ = track(w);
    w.focus_in();
}

void Focus::clear()
{
    if (in_focus_.valid()) {
        in_focus_.get().focus_out();
    }
    in_focus_ = std::shared_ptr<Widget*>{nullptr};
}

}  // namespace ox