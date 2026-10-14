#include <ox/widget.hpp>

#include <memory>

#include <ox/application.hpp>
#include <ox/focus.hpp>

namespace ox {

Widget::Widget(FocusPolicy fp, SizePolicy sp) : focus_policy{fp}, size_policy{sp} {}

Widget::Widget(Widget&& other)
    : focus_policy{other.focus_policy},
      size_policy{other.size_policy},
      visible{other.visible},
      enabled{other.enabled},
      bounds_{other.bounds_},
      focused_{std::exchange(other.focused_, false)},
      lifetime_{std::move(other.lifetime_)},
      application_{std::exchange(other.application_, nullptr)},
      timers_{std::move(other.timers_)}
{
    if (!lifetime_) { lifetime_ = std::make_shared<Widget*>(this); }
    *lifetime_ = this;
}

auto Widget::operator=(Widget&& other) -> Widget&
{
    if (this == &other) { return *this; }
    if (lifetime_) { *lifetime_ = nullptr; }
    for (auto const& entry : timers_) {
        if (auto timer = entry.lock()) {
            timer->deadline.reset();
            timer->owner = nullptr;
        }
    }
    focus_policy = other.focus_policy;
    size_policy = other.size_policy;
    visible = other.visible;
    enabled = other.enabled;
    focused_ = std::exchange(other.focused_, false);
    bounds_ = other.bounds_;

    application_ = std::exchange(other.application_, nullptr);
    timers_ = std::move(other.timers_);
    lifetime_ = std::move(other.lifetime_);
    if (!lifetime_) { lifetime_ = std::make_shared<Widget*>(this); }
    *lifetime_ = this;

    return *this;
}

void arrange(Widget& widget, Rect bounds)
{
    validate(bounds.size);
    auto const previous = std::exchange(widget.bounds_, bounds);
    widget.request_repaint();
    widget.on_arrange(previous);
}

void Widget::request_repaint()
{
    if (application_) { application_->request_repaint(); }
}

Widget::~Widget()
{
    for (auto const& entry : timers_) {
        if (auto timer = entry.lock()) {
            timer->deadline.reset();
            timer->owner = nullptr;
        }
    }
    if (lifetime_) { *lifetime_ = nullptr; }
}

}  // namespace ox