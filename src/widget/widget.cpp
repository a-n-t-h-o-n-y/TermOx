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
#include <cppurses/system/events/child_event.hpp>
#include <cppurses/system/events/delete_event.hpp>
#include <cppurses/system/events/disable_event.hpp>
#include <cppurses/system/events/enable_event.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/terminal.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/cursor_data.hpp>

namespace {

auto get_unique_id() -> std::uint16_t
{
    static std::mutex mtx;
    static auto current = std::uint16_t{0};
    std::lock_guard<std::mutex> lock{mtx};
    return ++current;
}

void post_child_polished(cppurses::Widget& w)
{
    using namespace cppurses;
    auto* parent = w.parent();
    if (parent != nullptr)
        System::post_event<Child_polished_event>(*parent, w);
}

}  // namespace

namespace cppurses {

Widget::Widget(std::string name)
    : name_{std::move(name)}, unique_id_{get_unique_id()}
{
    width_policy.policy_updated.connect([this] { post_child_polished(*this); });
    height_policy.policy_updated.connect(
        [this] { post_child_polished(*this); });
}

void Widget::enable(bool enable, bool post_child_polished_event)
{
    this->enable_and_post_events(enable, post_child_polished_event);
    // This counters the fact that layouts are notified of enabled events but
    // not disable events. So this has to disable children b/c layout will not.
    // if (not enable) {
    for (Widget& w : this->get_children()) {
        // if (w.enabled())
        w.enable(enable, post_child_polished_event);
    }
    // }
}

void Widget::close()
{
    std::unique_ptr<Widget> removed;
    if (this->parent() == nullptr)
        return;  // Can't delete a widget that is not owned by another Widget.
    removed = this->parent()->children_.remove(this);
    System::post_event<Delete_event>(*this, std::move(removed));
}

// This is here because including paint_event.hpp in widget.hpp causes issues.
void Widget::update() { System::post_event<Paint_event>(*this); }

auto Widget::generate_wallpaper() const -> Glyph
{
    auto bg_glyph =
        this->wallpaper ? *(this->wallpaper) : System::terminal.background();
    if (this->brush_paints_wallpaper())
        imprint(this->brush, bg_glyph.brush);
    return bg_glyph;
}

void Widget::install_event_filter(Widget& filter)
{
    if (&filter == this)
        return;
    auto const result = event_filters_.insert(&filter);
    if (result.second) {  // if insert happened
        // Remove filter from list on destruction of filter
        auto remove_on_destroy = sig::Slot<void()>{
            [this, &filter]() { this->remove_event_filter(filter); }};
        // In case *this has been destroyed and the filter has not.
        remove_on_destroy.track(this->destroyed);
        filter.destroyed.connect(remove_on_destroy);
    }
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
    if (post_child_polished_event and this->parent() != nullptr)
        System::post_event<Child_polished_event>(*this->parent(), *this);
    this->update();
}

}  // namespace cppurses
