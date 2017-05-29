#ifndef WIDGET_MODULE_WIDGET_HPP
#define WIDGET_MODULE_WIDGET_HPP

#include "painter_module/brush.hpp"
#include "painter_module/color.hpp"
#include "painter_module/geometry.hpp"
#include "painter_module/glyph.hpp"
#include "painter_module/paint_engine.hpp"
#include "system_module/events/mouse_event.hpp"
#include "system_module/key.hpp"
#include "system_module/object.hpp"
#include "widget_module/border.hpp"
#include "widget_module/coordinates.hpp"
#include "widget_module/focus_policy.hpp"
#include <aml/signals/signals.hpp>
#include <cstddef>
#include <memory>
#include <utility>

namespace cppurses {
class Clear_screen_event;
class Close_event;
class Enable_event;
class Focus_event;
class Hide_event;
class Key_event;
class Move_event;
class Paint_event;
class Resize_event;
class Show_event;

class Widget : public Object {
   public:
    Widget();
    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;
    Widget(Widget&&) = default;             // NOLINT
    Widget& operator=(Widget&&) = default;  // NOLINT
    ~Widget() override;

    // Global Coordinates - Includes border space.
    std::size_t x() const;
    std::size_t y() const;
    void set_x(std::size_t global_x);
    void set_y(std::size_t global_y);
    std::size_t width() const;  // Does not include border space
    std::size_t height() const;

    bool cursor() const { return cursor_enabled_; }
    void enable_cursor(bool enable = true) { cursor_enabled_ = enable; }
    void disable_cursor(bool disable = true) { cursor_enabled_ = !disable; }
    void move_cursor(std::size_t x, std::size_t y);
    void move_cursor(Coordinates c);
    void move_cursor_x(std::size_t x);
    void move_cursor_y(std::size_t y);
    std::size_t cursor_x() const { return cursor_position_.x; }
    std::size_t cursor_y() const { return cursor_position_.y; }
    Coordinates cursor_coordinates() const { return cursor_position_; }

    Border& border() { return border_; }
    const Border& border() const { return border_; }
    bool has_border() const { return border_.enabled(); }
    void enable_border();
    void disable_border();

    void set_brush(Brush brush);
    Brush& brush() { return default_brush_; }

    Glyph background_tile() { return background_tile_; }
    void set_background_tile(Glyph tile) { background_tile_ = std::move(tile); }

    bool has_focus() const { return focus_; }
    void set_focus(bool focus);
    void clear_focus() { this->set_focus(false); }
    Focus_policy focus_policy() const { return focus_policy_; }
    void set_focus_policy(Focus_policy policy) { focus_policy_ = policy; }

    void set_geometry(const Geometry& g);
    Geometry& geometry() { return geometry_; }
    const Geometry& geometry() const { return geometry_; }

    Size_policy& size_policy() { return geometry().size_policy(); }
    const Size_policy& size_policy() const { return geometry().size_policy(); }

    void update();

    bool has_coordinates(std::size_t global_x, std::size_t global_y) override;

    bool has_mouse_tracking() const { return mouse_tracking_; }
    void set_mouse_tracking(bool track) { mouse_tracking_ = track; }

    bool visible() const { return visible_; }
    void set_visible(bool visible);

    Paint_engine& paint_engine() const;
    void set_paint_engine(std::unique_ptr<Paint_engine> engine) {
        paint_engine_ = std::move(engine);
    }

    // Signals
    sig::Signal<void(std::size_t, std::size_t)> resized;
    sig::Signal<void(Coordinates)> moved;
    sig::Signal<void(Coordinates)> clicked;
    sig::Signal<void(Key)> key_pressed;
    sig::Signal<void(Widget*)> child_added;
    sig::Signal<void(Widget*)> child_removed;
    sig::Signal<void()> focused_in;
    sig::Signal<void()> focused_out;
    sig::Signal<void(Coordinates)> cursor_moved;
    sig::Signal<void(Color)> background_color_changed;
    sig::Signal<void(Color)> foreground_color_changed;

    // Slots
    sig::Slot<void()> close;
    sig::Slot<void()> hide;
    sig::Slot<void()> show;
    sig::Slot<void()> repaint;
    sig::Slot<void()> give_focus;
    sig::Slot<void()> update_me;
    sig::Slot<void(Mouse_button, Coordinates)> click_me;
    sig::Slot<void(Key)> keypress_me;

    sig::Slot<void(Color)> set_background;
    sig::Slot<void(Color)> set_foreground;

   protected:
    bool event(const Event& event) override;
    virtual bool move_event(const Move_event& event);
    virtual bool resize_event(const Resize_event& event);
    virtual bool paint_event(const Paint_event& event);
    virtual bool mouse_press_event(const Mouse_event& event);
    virtual bool mouse_release_event(const Mouse_event& event);
    virtual bool mouse_double_click_event(const Mouse_event& event);
    virtual bool wheel_event(const Mouse_event& event);
    virtual bool mouse_move_event(const Mouse_event& event);
    virtual bool key_press_event(const Key_event& event);
    virtual bool key_release_event(const Key_event& event);
    virtual bool close_event(const Close_event& event);
    virtual bool hide_event(const Hide_event& event);
    virtual bool show_event(const Show_event& event);
    bool enable_event(const Enable_event& event) override;
    virtual bool focus_event(const Focus_event& event);
    bool child_event(const Child_event& event) override;
    virtual bool clear_screen_event(const Clear_screen_event& event);

    void clear_screen();

    void set_background_(Color c);
    void set_foreground_(Color c);

    Coordinates position_;  // Top left corner relative to parent's Coordinates.
    Coordinates cursor_position_;
    bool cursor_enabled_{false};
    bool focus_{false};
    bool mouse_tracking_{false};
    bool visible_{true};
    Geometry geometry_{this};
    Border border_;
    Glyph background_tile_{" "};
    Brush default_brush_{background(Color::Black), foreground(Color::White)};
    Focus_policy focus_policy_{Focus_policy::None};
    std::unique_ptr<Paint_engine> paint_engine_{nullptr};

   private:
    void initialize();
};

}  // namespace cppurses

#endif  // WIDGET_MODULE_WIDGET_HPP
