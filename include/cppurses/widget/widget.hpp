#ifndef WIDGET_WIDGET_HPP
#define WIDGET_WIDGET_HPP
#include "painter/brush.hpp"
#include "painter/color.hpp"
#include "painter/geometry.hpp"
#include "painter/glyph.hpp"
#include "painter/paint_engine.hpp"
#include "system/events/mouse_event.hpp"
#include "system/key.hpp"
#include "system/event_handler.hpp"
#include "widget/border.hpp"
#include "widget/coordinates.hpp"
#include "widget/focus_policy.hpp"
#include <signals/signals.hpp>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <queue>

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

class Widget : public Event_handler {
   public:
    Widget();
    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;
    Widget(Widget&&) = delete;
    Widget& operator=(Widget&&) = delete;
    ~Widget() override;

    // Name
    std::string name() const;
    void set_name(const std::string& name);

    // Parent

    Widget* parent() const;
    void set_parent(Widget* parent);

    // Children
    template <typename T, typename... Args>
    T& make_child(Args&&... args) {
        this->add_child(std::make_unique<T>(std::forward<Args>(args)...));
        return static_cast<T&>(*children_.back());
    }

    void add_child(std::unique_ptr<Widget> child);
    std::vector<Widget*> children() const;

    bool has_child(Widget* child);

    template <typename T>
    T* find_child(const std::string& name) {
        return this->find_child_impl<T>(this, name);
    }

    template <typename T>
    const T* find_child(const std::string& name) const {
        return this->find_child_impl<T>(this, name);
    }

    // Global Coordinates - Includes border space
    std::size_t x() const;
    std::size_t y() const;
    void set_x(std::size_t global_x);
    void set_y(std::size_t global_y);
    std::size_t width() const;  // Does not include border space
    std::size_t height() const;
    bool has_coordinates(std::size_t global_x, std::size_t global_y);

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
    void set_vertical_policy(Size_policy::Policy policy, std::size_t hint);
    void set_vertical_policy(Size_policy::Policy policy);
    void set_horizontal_policy(Size_policy::Policy policy, std::size_t hint);
    void set_horizontal_policy(Size_policy::Policy policy);

    void update();

    bool has_mouse_tracking() const { return mouse_tracking_; }
    void set_mouse_tracking(bool track) { mouse_tracking_ = track; }

    bool visible() const { return visible_; }
    void set_visible(bool visible);

    Paint_engine& paint_engine() const;
    void set_paint_engine(std::unique_ptr<Paint_engine> engine) {
        paint_engine_ = std::move(engine);
    }

    // Signals
    sig::Signal<void(const std::string&)> name_changed;
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
    sig::Slot<void()> delete_later;
    sig::Slot<void()> enable;
    sig::Slot<void()> disable;
    sig::Slot<void()> close;
    sig::Slot<void()> hide;
    sig::Slot<void()> show;
    sig::Slot<void()> repaint;
    // sig::Slot<void()> give_focus;
    sig::Slot<void()> update_me;
    sig::Slot<void(Mouse_button, Coordinates)> click_me;
    sig::Slot<void(Key)> keypress_me;

    sig::Slot<void(Color)> set_background;
    sig::Slot<void(Color)> set_foreground;

    // Event Handling
    // bool event(const Event& event) override;
    bool move_event(std::size_t new_x,
                    std::size_t new_y,
                    std::size_t old_x,
                    std::size_t old_y) override;
    bool resize_event(std::size_t new_width,
                      std::size_t new_height,
                      std::size_t old_width,
                      std::size_t old_height) override;
    bool paint_event() override;
    // virtual bool mouse_press_event(Mouse_button button,
    //                                std::size_t global_x,
    //                                std::size_t global_y,
    //                                std::size_t local_x,
    //                                std::size_t local_y,
    //                                std::uint8_t device_id);
    // virtual bool mouse_release_event(Mouse_button button,
    //                                  std::size_t global_x,
    //                                  std::size_t global_y,
    //                                  std::size_t local_x,
    //                                  std::size_t local_y,
    //                                  std::uint8_t device_id);
    // virtual bool mouse_double_click_event(Mouse_button button,
    //                                       std::size_t global_x,
    //                                       std::size_t global_y,
    //                                       std::size_t local_x,
    //                                       std::size_t local_y,
    //                                       std::uint8_t device_id);
    // virtual bool mouse_wheel_event(Mouse_button button,
    //                                std::size_t global_x,
    //                                std::size_t global_y,
    //                                std::size_t local_x,
    //                                std::size_t local_y,
    //                                std::uint8_t device_id);
    // virtual bool mouse_move_event(Mouse_button button,
    //                               std::size_t global_x,
    //                               std::size_t global_y,
    //                               std::size_t local_x,
    //                               std::size_t local_y,
    //                               std::uint8_t device_id);
    bool key_press_event(Key key, char symbol) override;
    // virtual bool key_release_event(Key key, char symbol);
    bool close_event() override;
    // virtual bool hide_event();
    // virtual bool show_event();
    bool focus_in_event() override;
    bool focus_out_event() override;
    bool clear_screen_event() override;
    bool deferred_delete_event(Event_handler* to_delete) override;

    bool child_added_event(Event_handler* child) override;
    bool child_removed_event(Event_handler* child) override;
    bool child_polished_event(Event_handler* child) override;

    // Event Filter Handling
    // virtual bool move_event_filter(Widget* receiver,
    //                                std::size_t new_x,
    //                                std::size_t new_y,
    //                                std::size_t old_x,
    //                                std::size_t old_y);
    // virtual bool resize_event_filter(Widget* receiver,
    //                                  std::size_t new_width,
    //                                  std::size_t new_height,
    //                                  std::size_t old_width,
    //                                  std::size_t old_height);
    // virtual bool paint_event_filter(Widget* receiver);
    // virtual bool mouse_press_event_filter(Widget* receiver,
    //                                       Mouse_button button,
    //                                       std::size_t global_x,
    //                                       std::size_t global_y,
    //                                       std::size_t local_x,
    //                                       std::size_t local_y,
    //                                       std::uint8_t device_id);
    // virtual bool mouse_release_event_filter(Widget* receiver,
    //                                         Mouse_button button,
    //                                         std::size_t global_x,
    //                                         std::size_t global_y,
    //                                         std::size_t local_x,
    //                                         std::size_t local_y,
    //                                         std::uint8_t device_id);
    // virtual bool mouse_double_click_event_filter(Widget* receiver,
    //                                              Mouse_button button,
    //                                              std::size_t global_x,
    //                                              std::size_t global_y,
    //                                              std::size_t local_x,
    //                                              std::size_t local_y,
    //                                              std::uint8_t device_id);
    // virtual bool mouse_wheel_event_filter(Widget* receiver,
    //                                       Mouse_button button,
    //                                       std::size_t global_x,
    //                                       std::size_t global_y,
    //                                       std::size_t local_x,
    //                                       std::size_t local_y,
    //                                       std::uint8_t device_id);
    // virtual bool mouse_move_event_filter(Widget* receiver,
    //                                      Mouse_button button,
    //                                      std::size_t global_x,
    //                                      std::size_t global_y,
    //                                      std::size_t local_x,
    //                                      std::size_t local_y,
    //                                      std::uint8_t device_id);
    // virtual bool key_press_event_filter(Widget* receiver, Key key, char
    // symbol);
    // virtual bool key_release_event_filter(Widget* receiver,
    //                                       Key key,
    //                                       char symbol);
    // virtual bool close_event_filter(Widget* receiver);
    // virtual bool hide_event_filter(Widget* receiver);
    // virtual bool show_event_filter(Widget* receiver);
    // virtual bool focus_in_event_filter(Widget* receiver);
    // virtual bool focus_out_event_filter(Widget* receiver);
    // virtual bool clear_screen_event_filter(Widget* receiver);

   protected:
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
    std::string widget_name_;
    Widget* parent_ = nullptr;
    std::vector<std::unique_ptr<Widget>> children_;

    void initialize();
    void delete_child(Widget* child);

    // Breadth First Search for find_child()
    template <typename T, typename U>
    static auto find_child_impl(U u, const std::string& name)
        -> decltype(u->template find_child<T>(name)) {
        std::queue<U> queue_;
        queue_.push(u);
        while (!queue_.empty()) {
            auto current = queue_.front();
            queue_.pop();
            auto c_ptr = dynamic_cast<T*>(current);
            if (current->name() == name && c_ptr != nullptr) {
                return c_ptr;
            }
            auto children = current->children();
            std::for_each(std::begin(children), std::end(children),
                          [&](Widget* p) { queue_.push(p); });
        }
        return nullptr;
    }
};

}  // namespace cppurses

#endif  // WIDGET_WIDGET_HPP
