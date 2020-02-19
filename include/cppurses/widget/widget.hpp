#ifndef CPPURSES_WIDGET_WIDGET_HPP
#define CPPURSES_WIDGET_WIDGET_HPP
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iterator>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>

#include <optional/optional.hpp>
#include <signals/signal.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/screen_state.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/system/detail/focus.hpp>
#include <cppurses/system/events/child_event.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/cursor_data.hpp>
#include <cppurses/widget/detail/border_offset.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/size_policy.hpp>

namespace cppurses {
struct Area;

class Widget {
   public:
    sig::Signal<void(std::string const&)> name_changed;
    sig::Signal<void(std::size_t, std::size_t)> resized;
    sig::Signal<void(Point)> moved;
    sig::Signal<void(Widget*)> child_added;
    sig::Signal<void(Widget*)> child_removed;
    sig::Signal<void()> focused_in;
    sig::Signal<void()> focused_out;
    sig::Signal<void(Widget&)> destroyed;
    sig::Signal<void(Point)> clicked;
    sig::Signal<void(std::size_t, std::size_t)> clicked_xy;
    sig::Signal<void(Point)> click_released;
    sig::Signal<void(std::size_t, std::size_t)> click_released_xy;
    sig::Signal<void(Point)> double_clicked;
    sig::Signal<void(std::size_t, std::size_t)> double_clicked_xy;
    sig::Signal<void(Key::Code)> key_pressed;
    sig::Signal<void(Key::Code)> key_released;

    /// Describes the visual border of this Widget.
    Border border;

    /// Provides information on where the cursor is and if it is enabled.
    Cursor_data cursor{this};

    /// Describes how the width of this Widget should be modified by a Layout.
    Size_policy width_policy;

    /// Describes how the height of this Widget should be modified by a Layout.
    Size_policy height_policy;

    /// Describes how focus is given to this Widget.
    Focus_policy focus_policy{Focus_policy::None};

    /// Used to fill in empty space that is not filled in by paint_event().
    opt::Optional<Glyph> wallpaper;

    /// A Brush that is applied to every Glyph painted by this Widget.
    Brush brush{background(Color::Black), foreground(Color::White)};

    friend class Resize_event;
    friend class Move_event;

   public:
    /// Initialize with \p name.
    explicit Widget(std::string name = "");

    Widget(Widget const&) = delete;
    Widget(Widget&&)      = delete;
    Widget& operator=(Widget const&) = delete;
    Widget& operator=(Widget&&) = delete;

    virtual ~Widget()
    {
        if (detail::Focus::focus_widget() == this)
            detail::Focus::clear();
        destroyed(*this);
    }

    /// Return the name of the Widget.
    auto name() const -> std::string { return name_; }

    /// Return the ID number unique to this Widget.
    auto unique_id() const -> std::uint16_t { return unique_id_; }

    /// Set the identifying name of the Widget.
    void set_name(std::string name)
    {
        name_ = std::move(name);
        name_changed(name_);
    }

    /// Post an Enable_event to this widget, and all descendants.
    /** Will only post a Child_polished_event to the parent if requested. Useful
     *  for enabling a child Widget from a parent's Child_polished_event
     *  handler. This function can be overridden to change the implementation of
     *  what it means to enable a particular Widget type. For instance, if you
     *  have a Widget that has multiple child Widgets but only wants to display
     *  one at a time, then the enable() function can be overridden to enable
     *  itself and then only pass on enable() calls to the children that it
     *  wants to enable. */
    virtual void enable(bool enable                    = true,
                        bool post_child_polished_event = true);

    /// Post a Disable_event to this widget, and all descendants.
    /** Will only post a Child_polished_event to the parent if requested. Useful
     *  for disabling a child Widget from a parent's Child_polished_event
     *  handler. */
    void disable(bool disable = true, bool post_child_polished_event = true)
    {
        this->enable(!disable, post_child_polished_event);
    }

    /// Check whether the Widget is enabled.
    auto enabled() const -> bool { return enabled_; }

    /// Post a Delete_event to this, deleting the object when safe to do so.
    /** This Widget is immediately removed from its parent. The Widget is owned
     *  by a Delete_event object until it can be safely removed without leaving
     *  dangling references in event system. */
    void close();

    /// Return the Widget's parent pointer.
    /** The parent is the Widget that owns *this, it  is in charge of
     *  positioning and resizing this Widget. */
    auto parent() const -> Widget* { return parent_; }

    /// Return the global top left corner of this widget.
    auto top_left() const -> Point { return top_left_position_; }

    /// Return the global top left corner of this widget, not including border.
    auto inner_top_left() const -> Point
    {
        return {this->inner_x(), this->inner_y()};
    }

    /// x coordinate for the top left point of this Widget.
    /** Given with relation to the top left of the terminal screen. */
    auto x() const -> std::size_t { return top_left_position_.x; }

    /// y coordinate for the top left point of this Widget.
    /** Given with relation to the top left of the terminal screen. */
    auto y() const -> std::size_t { return top_left_position_.y; }

    /// x coordinate for the top left point of this Widget, beyond the Border.
    /** Given with relation to the top left of the terminal screen. This is the
     *  coordinate that marks the beginning of the space that is available for
     *  use by the Widget. */
    auto inner_x() const -> std::size_t
    {
        return top_left_position_.x + detail::Border_offset::west(*this);
    }

    /// y coordinate for the top left point of this Widget, beyond the Border.
    /** Given with relation to the top left of the terminal screen. This is the
     *  coordinate that marks the beginning of the space that is available for
     *  use by the Widget. */
    auto inner_y() const -> std::size_t
    {
        return top_left_position_.y + detail::Border_offset::north(*this);
    }

    /// Return the area the widget occupies, not including the Border.
    auto area() const -> Area { return {this->width(), this->height()}; }

    /// Return the inner width dimension, this does not include Border space.
    auto width() const -> std::size_t
    {
        return this->outer_width() - detail::Border_offset::east(*this) -
               detail::Border_offset::west(*this);
    }

    /// Return the inner height dimension, this does not include Border space.
    auto height() const -> std::size_t
    {
        return this->outer_height() - detail::Border_offset::north(*this) -
               detail::Border_offset::south(*this);
    }

    /// Return the area the widget occupies, including Border space.
    auto outer_area() const -> Area { return {outer_width_, outer_height_}; }

    /// Return the width dimension, this includes Border space.
    auto outer_width() const -> std::size_t { return outer_width_; }

    /// Return the height dimension, this includes Border space.
    auto outer_height() const -> std::size_t { return outer_height_; }

    // TODO remove virtual
    /// Post a paint event to this Widget.
    virtual void update();

    /// Install another Widget as an Event filter.
    /** The installed Widget will get the first go at processing the event with
     *  its filter event handler function. Widgets are installed in the order
     *  that calls to this function are made. They are handed the Event in that
     *  same order. If one Widget indicates that it has handled the event it can
     *  return true and no other Widget, including *this, will get the Event. */
    void install_event_filter(Widget& filter);

    /// Remove a Widget from the Event filter list.
    /** No-op if \p filter is not already installed. */
    void remove_event_filter(Widget& filter) { event_filters_.erase(&filter); }

    /// Return the list of Event filter Widgets.
    auto get_event_filters() const -> std::set<Widget*> const&
    {
        return event_filters_;
    }

    /// Enable animation on this Widget.
    /** Animated widgets receiver a Timer_event every \p period. This Timer
     *  Event should be used to update the state of the Widget. The animation
     *  system will also post a paint event to this Widget so the Widget can
     *  update itself. This is all handled on a separate thread from the main
     *  user input thread, and therefore has its own staged_changes object that
     *  it paints to to avoid shared data issues. */
    void enable_animation(Animation_engine::Period_t period)
    {
        System::animation_engine().register_widget(*this, period);
    }

    /// Enable variable animation on this Widget.
    /** Animated widgets receiver a Timer_event every \p period_func(). This
     *  enables a variable rate animation. */
    void enable_animation(
        std::function<Animation_engine::Period_t()> const& period_func)
    {
        System::animation_engine().register_widget(*this, period_func);
    }

    /// Turn off animation, no more Timer_events will be sent to this Widget.
    /** This Widget will be unregistered from the Animation_engine held by
     *  System. */
    void disable_animation()
    {
        System::animation_engine().unregister_widget(*this);
    }

    auto is_parent_of(Widget const* child) const -> bool
    {
        return children_.contains(child);
    }

    /// Get a range containing Widget& to each child.
    auto get_children() { return children_.get_children<Widget>(); }

    /// Get a const range containing Widget& to each child.
    auto get_children() const { return children_.get_children<Widget>(); }

    /// Return true if \p descendant exists within the widg. tree owned by this.
    auto is_ancestor_of(Widget const* descendant) const -> bool
    {
        return children_.contains_descendant(descendant);
    }

    /// Return container of all descendants of self_.
    auto get_descendants() const -> std::vector<Widget*>
    {
        return children_.get_descendants();
    }

    /// Return the number of children held by this Widget.
    auto child_count() const -> std::size_t { return children_.size(); }

    /// If true, the brush will apply to the wallpaper Glyph.
    auto brush_paints_wallpaper() const -> bool
    {
        return brush_paints_wallpaper_;
    }

    /// Set if the brush is applied to the wallpaper Glyph.
    void set_brush_paints_wallpaper(bool paints = true)
    {
        brush_paints_wallpaper_ = paints;
        this->update();
    }

    /// Return the wallpaper Glyph.
    /** The Glyph has the brush applied to it, if brush_paints_wallpaper is set
     *  to true. */
    auto generate_wallpaper() const -> Glyph;

    /// Return the current Screen_state of this Widget, as it appears.
    auto screen_state() -> detail::Screen_state& { return screen_state_; }

    /// Return the current Screen_state of this Widget, as it appears.
    auto screen_state() const -> detail::Screen_state const&
    {
        return screen_state_;
    }

    // - - - - - - - - - - - - - Event Handlers - - - - - - - - - - - - - - - -
    /// Handles Enable_event objects.
    virtual auto enable_event() -> bool { return true; }

    /// Handles Disable_event objects.
    virtual auto disable_event() -> bool { return true; }

    /// Handles Child_added_event objects.
    virtual auto child_added_event(Widget& child) -> bool
    {
        child_added(&child);
        return true;
    }

    /// Handles Child_removed_event objects.
    virtual auto child_removed_event(Widget& child) -> bool
    {
        child_removed(&child);
        return true;
    }

    /// Handles Child_polished_event objects.
    virtual auto child_polished_event(Widget & /* child */) -> bool
    {
        this->update();
        return true;
    }

    /// Handles Move_event objects.
    virtual auto move_event(Point new_position, Point /* old_position */)
        -> bool
    {
        moved(new_position);
        return true;
    }

    /// Handles Resize_event objects.
    virtual auto resize_event(Area /* new_size */, Area /* old_size */) -> bool
    {
        resized(outer_width_, outer_height_);
        this->update();
        return true;
    }

    /// Handles Mouse::Press objects.
    virtual auto mouse_press_event(Mouse::State const& mouse) -> bool
    {
        clicked(mouse.local);
        clicked_xy(mouse.local.x, mouse.local.y);
        return true;
    }

    /// Handles Mouse::Release objects.
    virtual auto mouse_release_event(Mouse::State const& mouse) -> bool
    {
        click_released(mouse.global);
        click_released_xy(mouse.global.x, mouse.global.y);
        return true;
    }

    /// Handles Mouse::Double_click objects.
    virtual auto mouse_double_click_event(Mouse::State const& mouse) -> bool
    {
        double_clicked(mouse.global);
        double_clicked_xy(mouse.global.x, mouse.global.y);
        return true;
    }

    // TODO Remove mouse_wheel_event
    /// Handles Mouse::Wheel objects.
    virtual auto mouse_wheel_event(Mouse::State const & /* mouse */) -> bool
    {
        return false;
    }

    /// Handles Mouse::Move objects.
    virtual auto mouse_move_event(Mouse::State const & /* mouse */) -> bool
    {
        return false;
    }

    /// Handles Key::Press objects.
    virtual auto key_press_event(Key::State const& keyboard) -> bool
    {
        key_pressed(keyboard.key);
        return true;
    }

    /// Handles Key::Release objects.
    virtual auto key_release_event(Key::State const& keyboard) -> bool
    {
        key_released(keyboard.key);
        return false;
    }

    /// Handles Focus_in_event objects.
    virtual auto focus_in_event() -> bool
    {
        focused_in();
        return true;
    }

    /// Handles Focus_out_event objects.
    virtual auto focus_out_event() -> bool
    {
        focused_out();
        return true;
    }

    /// Handles Delete_event objects.
    virtual auto delete_event() -> bool { return true; }

    /// Handles Paint_event objects.
    virtual auto paint_event() -> bool
    {
        Painter{*this}.border();
        return true;
    }

    /// Handles Timer_event objects.
    virtual auto timer_event() -> bool { return true; }

    // - - - - - - - - - - - Event Filter Handlers - - - - - - - - - - - - - - -
    /// Handles Child_added_event objects filtered from other Widgets.
    virtual auto child_added_event_filter(Widget& /* receiver */, Widget &
                                          /* child */) -> bool
    {
        return false;
    }

    /// Handles Child_removed_event objects filtered from other Widgets.
    virtual auto child_removed_event_filter(Widget& /* receiver */, Widget &
                                            /* child */) -> bool
    {
        return false;
    }

    /// Handles Child_polished_event objects filtered from other Widgets.
    virtual auto child_polished_event_filter(Widget& /* receiver */, Widget &
                                             /* child */) -> bool
    {
        return false;
    }

    /// Handles Enable_event objects filtered from other Widgets.
    virtual auto enable_event_filter(Widget & /* receiver */) -> bool
    {
        return false;
    }

    /// Handles Disable_event objects filtered from other Widgets.
    virtual auto disable_event_filter(Widget & /* receiver */) -> bool
    {
        return false;
    }

    /// Handles Move_event objects filtered from other Widgets.
    virtual auto move_event_filter(Widget& /* receiver */,
                                   Point /* new_position */,
                                   Point /* old_position */) -> bool
    {
        return false;
    }

    /// Handles Resize_event objects filtered from other Widgets.
    virtual auto resize_event_filter(Widget& /* receiver */,
                                     Area /* new_size */,
                                     Area /* old_size */) -> bool
    {
        return false;
    }

    /// Handles Mouse::Press objects filtered from other Widgets.
    virtual auto mouse_press_event_filter(Widget& /* receiver */,
                                          Mouse::State const &
                                          /* mouse */) -> bool
    {
        return false;
    }

    /// Handles Mouse::Release objects filtered from other Widgets.
    virtual auto mouse_release_event_filter(Widget& /* receiver */,
                                            Mouse::State const &
                                            /* mouse */) -> bool
    {
        return false;
    }

    /// Handles Mouse::Double_click objects filtered from other Widgets.
    virtual auto mouse_double_click_event_filter(Widget& /* receiver */,
                                                 Mouse::State const &
                                                 /* mouse */) -> bool
    {
        return false;
    }

    /// Handles Mouse::Wheel objects filtered from other Widgets.
    virtual auto mouse_wheel_event_filter(Widget& /* receiver */,
                                          Mouse::State const &
                                          /* mouse */) -> bool
    {
        return false;
    }

    /// Handles Mouse::Move objects filtered from other Widgets.
    virtual auto mouse_move_event_filter(Widget& /* receiver */,
                                         Mouse::State const &
                                         /* mouse */) -> bool
    {
        return false;
    }

    /// Handles Key::Press objects filtered from other Widgets.
    virtual auto key_press_event_filter(Widget& /* receiver */,
                                        Key::State const &
                                        /* keyboard */) -> bool
    {
        return false;
    }

    /// Handles Key::Release objects filtered from other Widgets.
    virtual auto key_release_event_filter(Widget& /* receiver */,
                                          Key::State const &
                                          /* keyboard */) -> bool
    {
        return false;
    }

    /// Handles Focus_in_event objects filtered from other Widgets.
    virtual auto focus_in_event_filter(Widget & /* receiver */) -> bool
    {
        return false;
    }

    /// Handles Focus_out_event objects filtered from other Widgets.
    virtual auto focus_out_event_filter(Widget & /* receiver */) -> bool
    {
        return false;
    }

    /// Handles Delete_event objects filtered from other Widgets.
    virtual auto delete_event_filter(Widget & /* receiver */) -> bool
    {
        return false;
    }

    /// Handles Paint_event objects filtered from other Widgets.
    virtual auto paint_event_filter(Widget & /* receiver */) -> bool
    {
        return false;
    }

    /// Handles Timer_event objects filtered from other Widgets.
    virtual auto timer_event_filter(Widget & /* receiver */) -> bool
    {
        return false;
    }

   protected:
    /// Enable this Widget and possibly notify the parent of the change.
    /** This function is useful if you want to override enable() function within
     *  your own derived Widget class. In those cases you could use this
     *  function to enable that Widget and then call enable() on only the
     *  children Widgets that you want enabled. */
    void enable_and_post_events(bool enable, bool post_child_polished_event);

   private:
    /// Owns all children of the owning Widget as a sequential list.
    /** Provides basic modification of the sequential list and access to its
     *  elements via Range and Const_range. */
    class Children {
        using List_t = std::vector<std::unique_ptr<Widget>>;
        /// Provides Widget_t const& access to underlying child objects.
        template <typename Widget_t>
        class Const_range {
            static_assert(std::is_base_of<Widget, Widget_t>::value,
                          "Widget_t must be a Widget type.");
            // TODO std::iterator is deprecated in C++17, use explicit type
            // aliases instead.
            // https://www.fluentcpp.com/2018/05/08/std-iterator-deprecated/
            class Iterator : public std::iterator<
                                 std::bidirectional_iterator_tag,
                                 typename List_t::const_iterator::value_type> {
               public:
                Iterator(List_t::const_iterator iter) : iter_{iter} {}
                auto operator*() const -> Widget_t const&
                {
                    return static_cast<Widget_t const&>(**iter_);
                }
                auto operator++() -> Iterator&
                {
                    ++iter_;
                    return *this;
                }
                auto operator--() -> Iterator&
                {
                    --iter_;
                    return *this;
                }
                auto operator==(Iterator other) const -> bool
                {
                    return this->iter_ == other.iter_;
                }
                auto operator!=(Iterator other) const -> bool
                {
                    return this->iter_ != other.iter_;
                }

               private:
                List_t::const_iterator iter_;
            };

           public:
            Const_range(List_t const& child_list) : child_list_{child_list} {}
            auto begin() const -> Iterator { return {std::begin(child_list_)}; }
            auto end() const -> Iterator { return {std::end(child_list_)}; }
            auto empty() const -> bool { return child_list_.empty(); }
            auto operator[](std::size_t index) const -> Widget_t const&
            {
                return static_cast<Widget_t const&>(*child_list_[index]);
            }
            auto front() const -> Widget_t const&
            {
                return static_cast<Widget_t const&>(*child_list_.front());
            }
            auto back() const -> Widget_t const&
            {
                return static_cast<Widget_t const&>(*child_list_.back());
            }

           private:
            List_t const& child_list_;
        };

        // TODO this is a view, it is non-owning and has reference semantics,
        // change the name to View?
        /// Provides Widget_t& access to underlying child objects.
        template <typename Widget_t>
        class Range {
            static_assert(std::is_base_of<Widget, Widget_t>::value,
                          "Widget_t must be a Widget type.");
            // TODO std::iterator is deprecated in C++17, as above
            class Iterator
                : public std::iterator<std::bidirectional_iterator_tag,
                                       typename List_t::iterator::value_type> {
               public:
                Iterator(List_t::iterator iter) : iter_{iter} {}
                auto operator*() const -> Widget_t&
                {
                    return static_cast<Widget_t&>(**iter_);
                }
                auto operator++() -> Iterator&
                {
                    ++iter_;
                    return *this;
                }
                auto operator--() -> Iterator&
                {
                    --iter_;
                    return *this;
                }
                auto operator==(Iterator other) const -> bool
                {
                    return this->iter_ == other.iter_;
                }
                auto operator!=(Iterator other) const -> bool
                {
                    return this->iter_ != other.iter_;
                }

               private:
                List_t::iterator iter_;
            };

           public:
            Range(List_t& child_list) : child_list_{child_list} {}
            auto begin() const -> Iterator { return {std::begin(child_list_)}; }
            auto end() const -> Iterator { return {std::end(child_list_)}; }
            auto empty() const -> bool { return child_list_.empty(); }
            auto size() const -> std::size_t { return child_list_.size(); }
            auto operator[](std::size_t index) const -> Widget_t&
            {
                return static_cast<Widget_t&>(*child_list_[index]);
            }
            auto front() const -> Widget_t&
            {
                return static_cast<Widget_t&>(*child_list_.front());
            }
            auto back() const -> Widget_t&
            {
                return static_cast<Widget_t&>(*child_list_.back());
            }

           private:
            List_t& child_list_;
        };

       public:
        Children(Widget* self) : self_{self} {}

        /// Move \p child directly before \p index in underlying vector.
        /** Throws std::invalid_argument if \p child is nullptr.
         *  Throws std::out_of_range if \p index > number of children.
         *  Returns a reference to the inserted Child_t object. */
        template <typename Child_t>
        auto insert(std::unique_ptr<Child_t> child_ptr, std::size_t index)
            -> Child_t&
        {
            assert_is_widget<Child_t>();
            if (child_ptr == nullptr)
                throw std::invalid_argument{"Children::insert: Child is Null"};
            if (index > this->size())
                throw std::out_of_range{"Children::insert: Index is Invalid"};

            auto const pos = std::next(std::cbegin(child_list_), index);
            Widget& child  = **child_list_.emplace(pos, std::move(child_ptr));
            child.set_parent(self_);
            child.enable(self_->enabled());
            System::post_event<Child_added_event>(*self_, child);
            return static_cast<Child_t&>(child);
        }

        template <typename Child_t>
        auto append(std::unique_ptr<Child_t> child_ptr) -> Child_t&
        {
            this->insert(std::move(child_ptr), this->size());
            return static_cast<Child_t&>(*child_list_.back());
        }

        /// Removes and returns the child pointed to by \p child_ptr.
        /** Throws std::invalid_argument if \p child_ptr doesn't point to a
         *  child of self_. */
        auto remove(Widget const* child_ptr) -> std::unique_ptr<Widget>
        {
            auto const child_iter = this->find_child(child_ptr);
            if (child_iter == std::end(child_list_))
                throw std::invalid_argument{"Children::remove: No Child Found"};
            return this->remove(child_iter);
        }

        /// Removes and returns the child at \p index in the child_list_.
        /** Throws std::out_of_range if \p index is not within child_list_. */
        auto remove(std::size_t index) -> std::unique_ptr<Widget>
        {
            if (index >= this->size())
                throw std::out_of_range{"Children::remove: Invalid Index"};
            auto const child_iter = std::next(std::begin(child_list_), index);
            return this->remove(child_iter);
        }

        /// Return true if \p child_ptr points to a child of self_.
        auto contains(Widget const* child_ptr) const -> bool
        {
            return this->find_child(child_ptr) != std::end(child_list_);
        }

        template <typename Widget_t>
        auto get_children() -> Range<Widget_t>
        {
            return Range<Widget_t>{child_list_};
        }

        template <typename Widget_t>
        auto get_children() const -> Const_range<Widget_t>
        {
            return Const_range<Widget_t>{child_list_};
        }

        /// Return true if \p descendant exists somewhere in the tree of self_.
        auto contains_descendant(Widget const* descendant) const -> bool
        {
            auto const begin = std::begin(child_list_);
            auto const end   = std::end(child_list_);
            return std::any_of(begin, end, [descendant](auto const& w) {
                return w.get() == descendant or w->is_ancestor_of(descendant);
            });
        }

        /// Return container of all descendants of self_.
        auto get_descendants() const -> std::vector<Widget*>
        {
            auto descendants = std::vector<Widget*>{};
            for (auto const& child_ptr : child_list_) {
                descendants.push_back(child_ptr.get());
                auto branch = child_ptr->get_descendants();
                descendants.insert(std::end(descendants), std::begin(branch),
                                   std::end(branch));
            }
            return descendants;
        }

        /// Return the number of children.
        auto size() const -> std::size_t { return child_list_.size(); }

       private:
        template <typename Child_t>
        constexpr void assert_is_widget()
        {
            static_assert(std::is_base_of<Widget, Child_t>::value,
                          "Child_t must be a Widget type");
        }

        /// Return iterator to \p child_ptr in child_list_, or end if not found.
        auto find_child(Widget const* child_ptr) const -> List_t::const_iterator
        {
            auto const end          = std::cend(child_list_);
            auto const begin        = std::cbegin(child_list_);
            auto const is_given_ptr = [child_ptr](auto const& w) -> bool {
                return w.get() == child_ptr;
            };
            return std::find_if(begin, end, is_given_ptr);
        }

        /// Return iterator to \p child_ptr in child_list_, or end if not found.
        auto find_child(Widget const* child_ptr) -> List_t::iterator
        {
            auto const end          = std::end(child_list_);
            auto const begin        = std::begin(child_list_);
            auto const is_given_ptr = [child_ptr](auto const& w) -> bool {
                return w.get() == child_ptr;
            };
            return std::find_if(begin, end, is_given_ptr);
        }

        /// Removes and returns child at \p child_iter, assumes is valid iter.
        auto remove(List_t::iterator child_iter) -> std::unique_ptr<Widget>
        {
            auto removed = std::unique_ptr<Widget>{std::move(*child_iter)};
            child_list_.erase(child_iter);
            removed->disable();
            System::post_event<Child_removed_event>(*self_, *removed);
            removed->set_parent(nullptr);
            return removed;
        }

       private:
        List_t child_list_;
        Widget* self_;  // The parent of the children
    };

   protected:
    Children children_ = {this};

   private:
    std::string name_;
    std::uint16_t const unique_id_;
    Widget* parent_              = nullptr;
    bool enabled_                = false;
    bool brush_paints_wallpaper_ = true;
    detail::Screen_state screen_state_;
    std::set<Widget*> event_filters_;

    // Top left point of *this, relative to the top left of the screen. Does not
    // account for borders.
    Point top_left_position_ = {0, 0};

    std::size_t outer_width_  = width_policy.hint();
    std::size_t outer_height_ = height_policy.hint();

   private:
    void set_x(std::size_t global_x) { top_left_position_.x = global_x; }

    void set_y(std::size_t global_y) { top_left_position_.y = global_y; }

    void set_parent(Widget* parent) { parent_ = parent; }
};
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGET_HPP
