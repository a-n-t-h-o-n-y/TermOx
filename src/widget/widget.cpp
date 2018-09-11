#include <cppurses/widget/widget.hpp>

#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <signals/signal.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/add_default_attributes.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/system/events/child_event.hpp>
#include <cppurses/system/events/delete_event.hpp>
#include <cppurses/system/events/disable_event.hpp>
#include <cppurses/system/events/enable_event.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/children_data.hpp>
#include <cppurses/widget/cursor_data.hpp>
#include <cppurses/widget/detail/border_offset.hpp>

namespace cppurses {
namespace detail {
class Screen_state;
}

Widget::Widget(std::string name) : name_{std::move(name)} {}

Widget::~Widget() {
    if (Focus::focus_widget() == this) {
        Focus::clear_focus();
    }
    destroyed(this);
}

void Widget::set_name(std::string name) {
    name_ = std::move(name);
    name_changed(name_);
}

std::string Widget::name() const {
    return name_;
}

void Widget::set_parent(Widget* parent) {
    parent_ = parent;
}

Widget* Widget::parent() const {
    return parent_;
}

void Widget::enable(bool enable, bool post_child_polished_event) {
    this->enable_and_post_events(enable, post_child_polished_event);
    for (std::unique_ptr<Widget>& w : this->children.children_) {
        w->enable(enable, post_child_polished_event);
    }
}

void Widget::disable(bool disable, bool post_child_polished_event) {
    this->enable(!disable, post_child_polished_event);
}

bool Widget::enabled() const {
    return enabled_;
}

void Widget::close() {
    std::unique_ptr<Widget> removed;
    if (this->parent() == nullptr) {
        // Can't delete a widget that is not owned by the system.
        return;
    } else {
        removed = this->parent()->children.remove(this);
    }
    System::post_event<Delete_event>(this, std::move(removed));
}

std::size_t Widget::x() const {
    return top_left_position_.x;
}

std::size_t Widget::y() const {
    return top_left_position_.y;
}

std::size_t Widget::inner_x() const {
    return top_left_position_.x + detail::Border_offset::west(*this);
}

std::size_t Widget::inner_y() const {
    return top_left_position_.y + detail::Border_offset::north(*this);
}

std::size_t Widget::width() const {
    return this->outer_width() - detail::Border_offset::east(*this) -
           detail::Border_offset::west(*this);
}

std::size_t Widget::height() const {
    return this->outer_height() - detail::Border_offset::north(*this) -
           detail::Border_offset::south(*this);
}

std::size_t Widget::outer_width() const {
    return outer_width_;
}

std::size_t Widget::outer_height() const {
    return outer_height_;
}

bool Widget::brush_paints_wallpaper() const {
    return brush_paints_wallpaper_;
}

void Widget::set_brush_paints_wallpaper(bool paints) {
    brush_paints_wallpaper_ = paints;
    this->update();
}

Glyph Widget::generate_wallpaper() const {
    Glyph background{this->wallpaper ? *(this->wallpaper)
                                     : System::terminal.background_tile()};
    if (this->brush_paints_wallpaper()) {
        detail::add_default_attributes(background, this->brush);
    }
    return background;
}

void Widget::update() {
    System::post_event<Paint_event>(this);
}

void Widget::install_event_filter(Widget* filter) {
    if (filter == this) {
        return;
    }
    // Remove filter from list on destruction of filter
    sig::Slot<void(Widget*)> remove_on_destroy{[this](Widget* being_destroyed) {
        this->remove_event_filter(being_destroyed);
    }};
    remove_on_destroy.track(this->destroyed);
    filter->destroyed.connect(remove_on_destroy);
    event_filters_.push_back(filter);
}

void Widget::remove_event_filter(Widget* filter) {
    auto begin = std::begin(event_filters_);
    auto end = std::end(event_filters_);
    auto position = std::find(begin, end, filter);
    if (position != end) {
        event_filters_.erase(position);
    }
}

const std::vector<Widget*>& Widget::get_event_filters() const {
    return event_filters_;
}

void Widget::enable_animation(Animation_engine::Period_t period) {
    System::animation_engine().register_widget(*this, period);
}

void Widget::enable_animation(
    const std::function<Animation_engine::Period_t()>& period_func) {
    System::animation_engine().register_widget(*this, period_func);
}

void Widget::disable_animation() {
    System::animation_engine().unregister_widget(*this);
}

void Widget::enable_and_post_events(bool enable,
                                    bool post_child_polished_event) {
    if (enabled_ != enable) {
        if (!enable) {
            System::post_event<Disable_event>(this);
        }
        enabled_ = enable;
        if (enable) {
            System::post_event<Enable_event>(this);
        }
        if (post_child_polished_event && this->parent() != nullptr) {
            System::post_event<Child_polished_event>(this->parent(), this);
        }
        this->update();
    }
}

void Widget::set_x(std::size_t global_x) {
    top_left_position_.x = global_x;
}

void Widget::set_y(std::size_t global_y) {
    top_left_position_.y = global_y;
}

detail::Screen_state& Widget::screen_state() {
    return screen_state_;
}

const detail::Screen_state& Widget::screen_state() const {
    return screen_state_;
}

}  // namespace cppurses
