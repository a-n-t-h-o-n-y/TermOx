#include <chrono>
#include <termox/widget/widget.hpp>

#include <chrono>
#include <cstdint>
#include <mutex>
#include <string>
#include <utility>

#include <signals_light/signal.hpp>

#include <termox/painter/brush.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/system/event.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/terminal.hpp>

namespace {

auto get_unique_id() -> std::uint16_t
{
    static auto mtx     = std::mutex{};
    static auto current = std::uint16_t{0};
    auto const lock     = std::scoped_lock{mtx};
    return ++current;
}

void post_child_polished(ox::Widget& w)
{
    using namespace ox;
    auto* parent = w.parent();
    if (parent != nullptr)
        System::post_event(Child_polished_event{*parent, w});
}

}  // namespace

namespace ox {

Widget::Widget(std::string name,
               Focus_policy focus_policy_,
               Size_policy width_policy_,
               Size_policy height_policy_,
               Brush brush_,
               Glyph wallpaper,
               bool brush_paints_wallpaper,
               Cursor cursor)
    : focus_policy{std::move(focus_policy_)},
      cursor{std::move(cursor)},
      width_policy{std::move(width_policy_)},
      height_policy{std::move(height_policy_)},
      brush{std::move(brush_)},
      brush_paints_wallpaper_{std::move(brush_paints_wallpaper)},
      name_{std::move(name)},
      wallpaper_{std::move(wallpaper)},
      unique_id_{get_unique_id()}
{
    width_policy.policy_updated.connect(
        [this] { ::post_child_polished(*this); });
    height_policy.policy_updated.connect(
        [this] { ::post_child_polished(*this); });
}

Widget::Widget(Parameters p)
    : Widget{std::move(p.name),
             std::move(p.focus_policy),
             std::move(p.width_policy),
             std::move(p.height_policy),
             std::move(p.brush),
             std::move(p.wallpaper),
             std::move(p.brush_paints_wallpaper),
             std::move(p.cursor)}
{}

void Widget::set_name(std::string name) { name_ = std::move(name); }

auto Widget::name() const -> std::string const& { return name_; }

auto Widget::unique_id() const -> std::uint16_t { return unique_id_; }

void Widget::set_wallpaper(Glyph g)
{
    wallpaper_ = g;
    this->update();
}

auto Widget::get_wallpaper() const -> Glyph { return wallpaper_; }

void Widget::enable(bool enable)
{
    if (enabled_ == enable)
        return;
    if (!enable)
        System::post_event(Disable_event{*this});
    enabled_ = enable;
    if (enable)
        System::post_event(Enable_event{*this});
}

void Widget::disable(bool disable) { this->enable(!disable); }

auto Widget::is_enabled() const -> bool { return enabled_; }

auto Widget::parent() const -> Widget* { return parent_; }

auto Widget::top_left() const -> Point { return top_left_position_; }

auto Widget::area() const -> Area { return area_; }

void Widget::update() { System::post_event(Paint_event{*this}); }

auto Widget::is_layout_type() const -> bool { return false; }

void Widget::install_event_filter(Widget& filter)
{
    if (&filter == this)
        return;
    auto const result = event_filters_.insert(&filter);
    if (result.second) {  // if insert happened
        // Remove filter from list on destruction of filter
        auto remove_on_destroy = sl::Slot<void()>{
            [this, &filter]() { this->remove_event_filter(filter); }};
        // In case *this has been destroyed and the filter has not.
        remove_on_destroy.track(this->lifetime);
        filter.deleted.connect(remove_on_destroy);
    }
}

void Widget::remove_event_filter(Widget& filter)
{
    event_filters_.erase(&filter);
}

auto Widget::get_event_filters() const -> std::set<Widget*> const&
{
    return event_filters_;
}

void Widget::enable_animation(std::chrono::milliseconds interval)
{
    if (is_animated_)
        return;
    System::enable_animation(*this, interval);
    is_animated_ = true;
}

void Widget::enable_animation(FPS fps)
{
    if (is_animated_)
        return;
    System::enable_animation(*this, fps);
    is_animated_ = true;
}

void Widget::disable_animation()
{
    if (!is_animated_)
        return;
    System::disable_animation(*this);
    is_animated_ = false;
}

auto Widget::is_animated() const -> bool { return is_animated_; }

auto Widget::get_descendants() const -> std::vector<Widget*>
{
    auto descendants = std::vector<Widget*>{};
    for (auto const& w_ptr : children_) {
        descendants.push_back(w_ptr.get());
        auto branch = w_ptr->get_descendants();
        descendants.insert(std::end(descendants), std::begin(branch),
                           std::end(branch));
    }
    return descendants;
}

void Widget::paint_wallpaper_with_brush(bool paints)
{
    brush_paints_wallpaper_ = paints;
    this->update();
}

auto Widget::paints_wallpaper_with_brush() const -> bool
{
    return brush_paints_wallpaper_;
}

auto Widget::generate_wallpaper() const -> Glyph
{
    auto bg_glyph = wallpaper_;
    if (this->paints_wallpaper_with_brush())
        bg_glyph.brush = merge(bg_glyph.brush, this->brush);
    return bg_glyph;
}

auto Widget::get_child_offset() const -> std::size_t { return child_offset_; }

auto Widget::child_count() const -> std::size_t { return children_.size(); }

auto Widget::enable_event() -> bool
{
    this->update();
    return true;
}

auto Widget::disable_event() -> bool { return true; }

auto Widget::child_added_event(Widget&) -> bool { return true; }

auto Widget::child_removed_event(Widget&) -> bool { return true; }

auto Widget::child_polished_event(Widget&) -> bool
{
    this->update();
    return true;
}

auto Widget::move_event(Point, Point) -> bool
{
    this->update();
    return true;
}

auto Widget::resize_event(Area, Area) -> bool
{
    this->update();
    return true;
}

auto Widget::mouse_press_event(Mouse const&) -> bool { return true; }

auto Widget::mouse_release_event(Mouse const&) -> bool { return true; }

auto Widget::mouse_wheel_event(Mouse const&) -> bool { return true; }

auto Widget::mouse_move_event(Mouse const&) -> bool { return true; }

auto Widget::key_press_event(Key) -> bool { return true; }

auto Widget::focus_in_event() -> bool { return true; }

auto Widget::focus_out_event() -> bool { return true; }

auto Widget::delete_event() -> bool { return true; }

auto Widget::paint_event(Painter&) -> bool { return true; }

auto Widget::timer_event() -> bool { return true; }

auto Widget::enable_event_filter(Widget&) -> bool { return false; }

auto Widget::disable_event_filter(Widget&) -> bool { return false; }

auto Widget::child_added_event_filter(Widget&, Widget&) -> bool
{
    return false;
}

auto Widget::child_removed_event_filter(Widget&, Widget&) -> bool
{
    return false;
}

auto Widget::child_polished_event_filter(Widget&, Widget&) -> bool
{
    return false;
}

auto Widget::move_event_filter(Widget&, Point, Point) -> bool { return false; }

auto Widget::resize_event_filter(Widget&, Area, Area) -> bool { return false; }

auto Widget::mouse_press_event_filter(Widget&, Mouse const&) -> bool
{
    return false;
}

auto Widget::mouse_release_event_filter(Widget&, Mouse const&) -> bool
{
    return false;
}

auto Widget::mouse_wheel_event_filter(Widget&, Mouse const&) -> bool
{
    return false;
}

auto Widget::mouse_move_event_filter(Widget&, Mouse const&) -> bool
{
    return false;
}

auto Widget::key_press_event_filter(Widget&, Key) -> bool { return false; }

auto Widget::focus_in_event_filter(Widget&) -> bool { return false; }

auto Widget::focus_out_event_filter(Widget&) -> bool { return false; }

auto Widget::delete_event_filter(Widget&) -> bool { return false; }

auto Widget::paint_event_filter(Widget&, Painter&) -> bool { return false; }

auto Widget::timer_event_filter(Widget&) -> bool { return false; }

void Widget::set_top_left(Point p) { top_left_position_ = p; }

void Widget::set_area(Area a) { area_ = a; }

void Widget::set_parent(Widget* parent) { parent_ = parent; }

auto widget(std::string name,
            Focus_policy focus_policy,
            Size_policy width_policy,
            Size_policy height_policy,
            Brush brush,
            Glyph wallpaper,
            bool brush_paints_wallpaper,
            Cursor cursor) -> std::unique_ptr<Widget>
{
    return std::make_unique<Widget>(
        std::move(name), std::move(focus_policy), std::move(width_policy),
        std::move(height_policy), std::move(brush), std::move(wallpaper),
        std::move(brush_paints_wallpaper), std::move(cursor));
}

auto widget(Widget::Parameters p) -> std::unique_ptr<Widget>
{
    return std::make_unique<Widget>(std::move(p));
}

}  // namespace ox
