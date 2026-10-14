#include <ox/widget.hpp>

#include <memory>

#include <ox/application.hpp>
#include <ox/focus.hpp>

namespace ox {

Widget::Widget(FocusPolicy fp, SizePolicy width, SizePolicy height)
    : focus_policy{fp}, width_policy{width}, height_policy{height}
{}

Widget::Widget(Widget&& other)
    : focus_policy{other.focus_policy},
      width_policy{other.width_policy},
      height_policy{other.height_policy},
      visible{other.visible},
      enabled{other.enabled},
      overlay{other.overlay},
      modal{other.modal},
      overlay_order{other.overlay_order},
      shortcuts{std::move(other.shortcuts)},
      disabled_style{other.disabled_style},
      bounds_{other.bounds_},
      focused_{std::exchange(other.focused_, false)},
      input_enabled_{other.input_enabled_},
      lifetime_{std::move(other.lifetime_)},
      application_{std::exchange(other.application_, nullptr)},
      timers_{std::move(other.timers_)},
      animations_{std::move(other.animations_)}
{
    if (!lifetime_) { lifetime_ = std::make_shared<Widget*>(this); }
    *lifetime_ = this;
}

auto Widget::operator=(Widget&& other) -> Widget&
{
    if (this == &other) { return *this; }
    for (auto const& entry : animations_) {
        if (auto a = entry.lock()) {
            a->owner = nullptr;
            a->tween.reset(a->tween.target());
        }
    }
    if (lifetime_) { *lifetime_ = nullptr; }
    for (auto const& entry : timers_) {
        if (auto timer = entry.lock()) {
            timer->deadline.reset();
            timer->owner = nullptr;
        }
    }
    focus_policy = other.focus_policy;
    width_policy = other.width_policy;
    height_policy = other.height_policy;
    visible = other.visible;
    enabled = other.enabled;
    overlay = other.overlay;
    modal = other.modal;
    overlay_order = other.overlay_order;
    shortcuts = std::move(other.shortcuts);
    disabled_style = other.disabled_style;
    focused_ = std::exchange(other.focused_, false);
    input_enabled_ = other.input_enabled_;
    bounds_ = other.bounds_;

    application_ = std::exchange(other.application_, nullptr);
    animations_ = std::move(other.animations_);
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

void Widget::request_layout()
{
    if (application_) { application_->request_layout(); }
}

auto Widget::clipboard() -> Clipboard&
{
    if (!application_) {
        throw std::logic_error{"Clipboard requires an attached widget"};
    }
    return application_->clipboard;
}
auto Widget::theme() const -> Theme const&
{
    static auto const plain = Theme::plain();
    return application_ ? application_->theme() : plain;
}
auto Widget::glyph_width_policy() const -> esc::WidthPolicy
{
    return application_ ? application_->loop().screen.width_policy()
                        : esc::WidthPolicy{};
}

auto Widget::screen_size() const -> Area
{ return application_ ? application_->loop().screen.size() : size(); }
auto Widget::global_bounds() const -> Rect
{ return application_ ? application_->bounds(*this) : bounds(); }
auto Widget::request_focus() -> bool { return request_focus(*this); }
auto Widget::request_focus(Widget& target) -> bool
{
    if (!application_ || !application_->is_interactive(target)) { return false; }
    application_->set_focus(target);
    return true;
}

void Widget::bring_to_front()
{
    if (application_) { application_->raise_overlay(*this); }
}

auto Widget::capture_pointer(MouseButton button) -> bool
{ return application_ && application_->capture_pointer(*this, button); }
void Widget::release_pointer()
{
    if (application_) { application_->release_pointer(*this); }
}
auto Widget::is_pointer_captured() const -> bool
{ return application_ && application_->pointer_capture() == track(*this); }

Widget::~Widget()
{
    for (auto const& entry : timers_) {
        if (auto timer = entry.lock()) {
            timer->deadline.reset();
            timer->owner = nullptr;
        }
    }
    for (auto const& entry : animations_) {
        if (auto a = entry.lock()) {
            a->owner = nullptr;
            a->tween.reset(a->tween.target());
        }
    }
    if (lifetime_) { *lifetime_ = nullptr; }
}

}  // namespace ox