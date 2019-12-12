#include <cppurses/widget/widget.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

#include <signals/signal.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/system/events/child_event.hpp>
#include <cppurses/system/events/delete_event.hpp>
#include <cppurses/system/events/disable_event.hpp>
#include <cppurses/system/events/enable_event.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/terminal.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/cursor_data.hpp>

namespace {
auto get_unique_id() -> std::uint16_t
{
    static std::mutex mtx;
    static std::uint16_t current{0};
    std::lock_guard<std::mutex> lock{mtx};
    return ++current;
}
}  // namespace

namespace cppurses {
namespace detail {
class Screen_state;
}  // namespace detail

Widget::Widget(std::string name)
    : name_{std::move(name)}, unique_id_{get_unique_id()}
{}

Widget::~Widget()
{
    if (Focus::focus_widget() == this)
        Focus::clear();
    destroyed(*this);
}

void Widget::set_name(std::string name)
{
    name_ = std::move(name);
    name_changed(name_);
}

void Widget::enable(bool enable, bool post_child_polished_event)
{
    this->enable_and_post_events(enable, post_child_polished_event);
    for (Widget& w : this->get_children()) {
        w.enable(enable, post_child_polished_event);
    }
}

void Widget::close()
{
    std::unique_ptr<Widget> removed;
    if (this->parent() == nullptr)
        return;  // Can't delete a widget that is not owned by another Widget.
    else
        removed = this->parent()->children_.remove(this);
    System::post_event<Delete_event>(*this, std::move(removed));
}

auto Widget::generate_wallpaper() const -> Glyph
{
    Glyph background{this->wallpaper ? *(this->wallpaper)
                                     : System::terminal.background()};
    if (this->brush_paints_wallpaper())
        imprint(this->brush, background.brush);
    return background;
}

void Widget::update() { System::post_event<Paint_event>(*this); }

void Widget::install_event_filter(Widget& filter)
{
    if (&filter == this)
        return;
    auto const result = event_filters_.insert(&filter);
    if (result.second) {  // if insert happened
        // Remove filter from list on destruction of filter
        auto remove_on_destroy =
            sig::Slot<void(Widget&)>{[this](Widget& being_destroyed) {
                this->remove_event_filter(being_destroyed);
            }};
        remove_on_destroy.track(this->destroyed);
        filter.destroyed.connect(remove_on_destroy);
    }
}

void Widget::remove_event_filter(Widget& filter)
{
    event_filters_.erase(&filter);
}

void Widget::enable_animation(Animation_engine::Period_t period)
{
    System::animation_engine().register_widget(*this, period);
}

void Widget::enable_animation(
    std::function<Animation_engine::Period_t()> const& period_func)
{
    System::animation_engine().register_widget(*this, period_func);
}

void Widget::disable_animation()
{
    System::animation_engine().unregister_widget(*this);
}

void Widget::enable_and_post_events(bool enable, bool post_child_polished_event)
{
    if (enabled_ == enable)
        return;
    if (!enable)
        System::post_event<Disable_event>(*this);
    enabled_ = enable;
    if (enable)
        System::post_event<Enable_event>(*this);
    if (post_child_polished_event && this->parent() != nullptr)
        System::post_event<Child_polished_event>(*this->parent(), *this);
    this->update();
}
}  // namespace cppurses
