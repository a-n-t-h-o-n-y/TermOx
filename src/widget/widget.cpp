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
    for (Widget& w : this->get_children())
        w.enable(enable, post_child_polished_event);
}

void Widget::close()
{
    if (this->parent() == nullptr)
        return;  // Can't delete a widget that is not owned by another Widget.
    this->parent()->children_.erase(this);
}

void Widget::Children::erase(Widget const* child)
{
    auto const child_iter = this->find_impl(child);
    if (child_iter == std::end(child_list_))
        throw std::invalid_argument{"Children::remove: No Child Found"};
    std::unique_ptr<Widget> removed = this->remove_impl(child_iter);
    System::post_event<Delete_event>(*self_, std::move(removed));
}

void Widget::Children::erase(std::size_t index)
{
    this->erase(std::next(std::begin(child_list_), index)->get());
}

void Widget::Children::clear()
{
    for (auto& child : child_list_) {
        auto removed = std::move(child);
        removed->disable();
        System::post_event<Child_removed_event>(*self_, *removed);
        removed->set_parent(nullptr);
        System::post_event<Delete_event>(*self_, std::move(removed));
    }
    child_list_.clear();
}

// This is here because including paint_event.hpp in widget.hpp causes issues.
void Widget::update() { System::post_event<Paint_event>(*this); }

auto Widget::generate_wallpaper() const -> Glyph
{
    auto bg_glyph = wallpaper_ ? *(wallpaper_) : System::terminal.background();
    if (this->does_paint_wallpaper_with_brush())
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
