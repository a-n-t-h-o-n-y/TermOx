#ifndef CPPURSES_WIDGET_WIDGET_HPP
#define CPPURSES_WIDGET_WIDGET_HPP
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>

#include <signals/signal.hpp>

#include <cppurses/common/map_iterator.hpp>
#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/painter/trait.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/system/detail/focus.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/mouse.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/cursor_data.hpp>
#include <cppurses/widget/detail/border_offset.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/size_policy.hpp>

namespace cppurses {

class Widget {
   private:
    template <typename Signature>
    using Signal = sig::Signal<Signature>;

   public:
    // Event Signals - Alternatives to overriding virtual event handlers.
    /* Called after event handlers are invoked. Parameters are in same order as
     * matching event handler function's parameters. */
    Signal<void()> enabled;
    Signal<void()> disabled;
    Signal<void(Widget&)> child_added;
    Signal<void(Widget&)> child_removed;
    Signal<void(Widget&)> child_polished;
    Signal<void(Point const&, Point const&)> moved;
    Signal<void(Area const&, Area const&)> resized;
    Signal<void(Mouse const&)> mouse_pressed;
    Signal<void(Mouse const&)> mouse_released;
    Signal<void(Mouse const&)> mouse_double_clicked;
    Signal<void(Mouse const&)> mouse_wheel_scrolled;
    Signal<void(Mouse const&)> mouse_moved;
    Signal<void(Key)> key_pressed;
    Signal<void()> focused_in;
    Signal<void()> focused_out;
    Signal<void()> destroyed;
    Signal<void()> painted;
    Signal<void()> timer;

    // TODO add rest of _filter signals
    Signal<void(Widget&, Mouse const&)> mouse_wheel_filter;

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

    /// A Brush that is applied to every Glyph painted by this Widget.
    Brush brush{background(Color::Background), foreground(Color::Foreground)};

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
    }

   public:
    /// Set the identifying name of the Widget.
    void set_name(std::string name) { name_ = std::move(name); }

    /// Return the name of the Widget.
    auto name() const -> std::string const& { return name_; }

    /// Return the ID number unique to this Widget.
    auto unique_id() const -> std::uint16_t { return unique_id_; }

    /// Used to fill in empty space that is not filled in by paint_event().
    void set_wallpaper(std::optional<Glyph> g)
    {
        wallpaper_ = g;
        this->update();
    }

    /// Return the currently in use wallpaper or std::nullopt if none.
    auto get_wallpaper() const -> std::optional<Glyph> { return wallpaper_; }

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
    auto is_enabled() const -> bool { return enabled_; }

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
    auto outer_area() const -> Area { return outer_area_; }

    /// Return the width dimension, this includes Border space.
    auto outer_width() const -> std::size_t { return outer_area_.width; }

    /// Return the height dimension, this includes Border space.
    auto outer_height() const -> std::size_t { return outer_area_.height; }

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
     *  Event should be used to update the state of the Widget. This is all
     *  handled on a separate thread from the main user input thread, and has
     *  its own staged_changes object that it paints to to avoid shared data
     *  issues. */
    void enable_animation(Animation_engine::Period_t period)
    {
        System::animation_engine().register_widget(*this, period);
    }

    /// Enable animation with a frames-per-second value.
    void enable_animation(FPS fps)
    {
        System::animation_engine().register_widget(*this, fps);
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
    auto get_children() { return Children::View<Widget, Children>{children_}; }

    /// Get a const range containing Widget& to each child.
    auto get_children() const
    {
        return Children::View<Widget, Children const>{children_};
    }

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

    /// Return the index of the first child displayed by this Widget.
    auto child_offset() const -> std::size_t { return children_.get_offset(); }

    /// If true, the brush will apply to the wallpaper Glyph.
    auto does_paint_wallpaper_with_brush() const -> bool
    {
        return brush_paints_wallpaper_;
    }

    /// Set if the brush is applied to the wallpaper Glyph.
    void paint_wallpaper_with_brush(bool paints = true)
    {
        brush_paints_wallpaper_ = paints;
        this->update();
    }

    /// Return the wallpaper Glyph.
    /** The Glyph has the brush applied to it, if brush_paints_wallpaper is set
     *  to true. */
    auto generate_wallpaper() const -> Glyph;

    /// Return the current Screen_descriptor of this Widget, as it appears.
    auto screen_state() -> detail::Screen_descriptor& { return screen_state_; }

    /// Return the current Screen_descriptor of this Widget, as it appears.
    auto screen_state() const -> detail::Screen_descriptor const&
    {
        return screen_state_;
    }

    // - - - - - - - - - - - - - Event Handlers - - - - - - - - - - - - - - - -
    /// Handles Enable_event objects.
    virtual auto enable_event() -> bool
    {
        this->update();
        enabled();
        return true;
    }

    /// Handles Disable_event objects.
    virtual auto disable_event() -> bool
    {
        disabled();
        return true;
    }

    /// Handles Child_added_event objects.
    virtual auto child_added_event(Widget& child) -> bool
    {
        child_added(child);
        return true;
    }

    /// Handles Child_removed_event objects.
    virtual auto child_removed_event(Widget& child) -> bool
    {
        child_removed(child);
        return true;
    }

    /// Handles Child_polished_event objects.
    virtual auto child_polished_event(Widget& child) -> bool
    {
        this->update();
        child_polished(child);
        return true;
    }

    /// Handles Move_event objects.
    virtual auto move_event(Point new_position, Point old_position) -> bool
    {
        this->update();
        moved(new_position, old_position);
        return true;
    }

    /// Handles Resize_event objects.
    virtual auto resize_event(Area new_size, Area old_size) -> bool
    {
        this->update();
        resized(new_size, old_size);
        return true;
    }

    /// Handles Mouse_press_event objects.
    virtual auto mouse_press_event(Mouse const& m) -> bool
    {
        mouse_pressed(m);
        return true;
    }

    /// Handles Mouse_release_event objects.
    virtual auto mouse_release_event(Mouse const& m) -> bool
    {
        mouse_released(m);
        return true;
    }

    /// Handles Mouse_double_click_event objects.
    virtual auto mouse_double_click_event(Mouse const& m) -> bool
    {
        mouse_double_clicked(m);
        return true;
    }

    /// Handles Mouse_wheel_event objects.
    virtual auto mouse_wheel_event(Mouse const& m) -> bool
    {
        mouse_wheel_scrolled(m);
        return true;
    }

    /// Handles Mouse_move_event objects.
    virtual auto mouse_move_event(Mouse const& m) -> bool
    {
        mouse_moved(m);
        return false;
    }

    /// Handles Key_press_event objects.
    virtual auto key_press_event(Key k) -> bool
    {
        key_pressed(k);
        return true;
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
    virtual auto delete_event() -> bool
    {
        destroyed();
        return true;
    }

    /// Handles Paint_event objects.
    virtual auto paint_event() -> bool
    {
        Painter{*this}.border();
        painted();
        return true;
    }

    /// Handles Timer_event objects.
    virtual auto timer_event() -> bool
    {
        timer();
        return true;
    }

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

    /// Handles Mouse_press_event objects filtered from other Widgets.
    virtual auto mouse_press_event_filter(Widget& receiver, Mouse const& m)
        -> bool
    {
        mouse_wheel_filter(receiver, m);
        return false;
    }

    /// Handles Mouse_release_event objects filtered from other Widgets.
    virtual auto mouse_release_event_filter(Widget& /* receiver */,
                                            Mouse const &
                                            /* m */) -> bool
    {
        return false;
    }

    /// Handles Mouse_double_click_event objects filtered from other Widgets.
    virtual auto mouse_double_click_event_filter(Widget& /* receiver */,
                                                 Mouse const &
                                                 /* m */) -> bool
    {
        return false;
    }

    /// Handles Mouse_wheel_event objects filtered from other Widgets.
    virtual auto mouse_wheel_event_filter(Widget& /* receiver */, Mouse const &
                                          /* m */) -> bool
    {
        return false;
    }

    /// Handles Mouse_move_event objects filtered from other Widgets.
    virtual auto mouse_move_event_filter(Widget& /* receiver */, Mouse const &
                                         /* m */) -> bool
    {
        return false;
    }

    /// Handles Key_press_event objects filtered from other Widgets.
    virtual auto key_press_event_filter(Widget& /* receiver */, Key const &
                                        /* k */) -> bool
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

   public:
    /// Owns all children of the owning Widget as a sequential list.
    /** Provides a non-modifying View type. */
    class Children {
       public:
        using value_type = Widget;
        using List_t     = std::vector<std::unique_ptr<Widget>>;

        /// Provides limited view of the underlying Children object.
        /** Children_t for const/non-const versions. */
        template <typename Widget_t, typename Children_t>
        class View {
            static_assert(std::is_base_of<Widget, Widget_t>::value,
                          "Widget_t must be a Widget type.");

           public:
            using value_type = Widget_t;

           public:
            explicit View(Children_t& children) : children_{children} {}

            auto begin() const { return children_.template begin<Widget_t>(); }

            auto end() const { return children_.template end<Widget_t>(); }

            auto empty() const -> bool { return children_.empty(); }

            auto size() const -> std::size_t { return children_.size(); }

            auto operator[](std::size_t index) const -> auto&
            {
                return children_.template operator[]<Widget_t>(index);
            }

            auto front() const -> auto&
            {
                return children_.template front<Widget_t>();
            }

            auto back() const -> auto&
            {
                return children_.template back<Widget_t>();
            }

            /// Find first child satisfying \p pred.
            /** \p pred takes a const Widget_t reference and returns a bool
             *  Returns nullptr if no child is found. */
            template <typename Unary_predicate_t>
            auto find(Unary_predicate_t&& pred) const -> decltype(auto)
            {
                auto const iter =
                    std::find_if(this->begin(), this->end(),
                                 std::forward<Unary_predicate_t>(pred));
                return iter == this->end() ? nullptr : &(*iter);
            }

            /// Find a child's position given its pointer.
            /** Returns Widget::child_count() if no child is found. */
            auto find_by_pointer(Widget const* child) const -> std::size_t
            {
                auto const iter = std::find_if(
                    this->begin(), this->end(),
                    [child](Widget const& x) { return &x == child; });
                return std::distance(this->begin(), iter);
            }

           private:
            Children_t& children_;
        };

       public:
        explicit Children(Widget* self) : self_{self} {}

       public:  // Modifiers
        /// Move \p child to \p index in underlying vector.
        /** Inserts at end of container if \p index is out of range.
         *  Returns a reference to the inserted Child_t object. */
        template <typename Child_t>
        auto insert(std::unique_ptr<Child_t> child, std::size_t index)
            -> Child_t&
        {
            assert_is_widget<Child_t>();
            if (index > this->size())
                index = this->size();
            Widget& inserted_child = this->insert_impl(std::move(child), index);
            this->init_new_child(inserted_child);
            return static_cast<Child_t&>(inserted_child);
        }

        template <typename Child_t>
        auto insert(std::unique_ptr<Child_t> child, List_t::iterator at)
            -> Child_t&
        {
            Child_t& result = *child;
            child_list_.insert(at, std::move(child));
            return result;
        }

        /// Removes and returns the child pointed to by \p child.
        /** Throws std::invalid_argument if \p child isn't a child. */
        auto remove(Widget const* child) -> std::unique_ptr<Widget>
        {
            auto const child_iter = this->find_impl(child);
            if (child_iter == std::end(child_list_))
                throw std::invalid_argument{"Children::remove: No Child Found"};
            return this->remove_impl(child_iter);
        }

        /// Removes and returns the child at \p index in the child_list_.
        /** Throws std::out_of_range if \p index is out of range.. */
        auto remove(std::size_t index) -> std::unique_ptr<Widget>
        {
            if (index >= this->size())
                throw std::out_of_range{"Children::remove: Invalid Index"};
            auto const child_iter = std::next(std::begin(child_list_), index);
            return this->remove_impl(child_iter);
        }

        /// Removes and deletes a child.
        void erase(Widget const* child);

        /// Removes and deletes a child.
        void erase(std::size_t index);

        /// Removes and sends delete event each child.
        void clear();

        /// Swap two child widgets, no index range check.
        void swap(std::size_t index_a, std::size_t index_b)
        {
            auto const begin = std::begin(child_list_);
            std::iter_swap(std::next(begin, index_a),
                           std::next(begin, index_b));
        }

       public:  // Accessors
        /// Returns the begin iterator to the child widgets.
        /** Returns a reference to Widget_t when dereferenced. */
        template <typename Widget_t>
        auto begin()
        {
            return this->make_deref_and_cast_iter<Widget_t>(
                std::begin(child_list_));
        }

        /// Returns the begin iterator to the child widgets.
        /** Returns a reference to Widget_t when dereferenced. */
        template <typename Widget_t>
        auto begin() const
        {
            return this->make_const_deref_and_cast_iter<Widget_t>(
                std::cbegin(child_list_));
        }

        template <typename Widget_t>
        auto end()
        {
            return this->make_deref_and_cast_iter<Widget_t>(
                std::end(child_list_));
        }

        template <typename Widget_t>
        auto end() const
        {
            return this->make_const_deref_and_cast_iter<Widget_t>(
                std::cend(child_list_));
        }

        /// Return true is contains no child Widgets.
        auto empty() const -> bool { return child_list_.empty(); }

        /// Return the number of children.
        auto size() const -> std::size_t { return child_list_.size(); }

        template <typename Widget_t>
        auto operator[](std::size_t index) -> Widget_t&
        {
            return static_cast<Widget_t&>(*child_list_[index]);
        }

        template <typename Widget_t>
        auto operator[](std::size_t index) const -> Widget_t const&
        {
            return static_cast<Widget_t const&>(*child_list_[index]);
        }

        template <typename Widget_t>
        auto front() -> Widget_t&
        {
            return static_cast<Widget_t&>(*child_list_.front());
        }

        template <typename Widget_t>
        auto front() const -> Widget_t const&
        {
            return static_cast<Widget_t const&>(*child_list_.front());
        }

        template <typename Widget_t>
        auto back() -> Widget_t&
        {
            return static_cast<Widget_t&>(*child_list_.back());
        }

        template <typename Widget_t>
        auto back() const -> Widget_t const&
        {
            return static_cast<Widget_t const&>(*child_list_.back());
        }

        /// Return the child offset, the first widget included in the layout.
        auto get_offset() const -> std::size_t { return offset_; }

        /// Sets the child Widget offset, does not do bounds checking.
        void set_offset(std::size_t index) { offset_ = index; }

        /// Return true if \p child points to a child of self_.
        auto contains(Widget const* child) const -> bool
        {
            return this->find_impl(child) != std::end(child_list_);
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

       private:
        List_t child_list_;
        Widget* self_;  // The owning parent Widget

        /// Index into child_list_ that is the beginning of the layout display.
        std::size_t offset_ = 0uL;

        static auto constexpr deref = [](auto& ptr) -> auto& { return *ptr; };

        static auto constexpr deref_const = [](auto const& ptr) -> auto const&
        {
            return *ptr;
        };

        template <typename Widget_t>
        static auto constexpr cast = [](auto& w) -> auto&
        {
            return static_cast<Widget_t&>(w);
        };

        template <typename Widget_t>
        static auto constexpr cast_const = [](auto const& w) -> auto const&
        {
            return static_cast<Widget_t const&>(w);
        };

       private:
        template <typename Child_t>
        constexpr void assert_is_widget()
        {
            static_assert(std::is_base_of<Widget, Child_t>::value,
                          "Child_t must be a Widget type");
        }

        auto insert_impl(std::unique_ptr<Widget> child, std::size_t index)
            -> Widget&
        {
            auto const pos = std::next(std::cbegin(child_list_), index);
            return **(child_list_.emplace(pos, std::move(child)));
        }

        /// Setup \p w to be a child of self_.
        void init_new_child(Widget& w);

        /// Removes and returns child at \p child_iter, assumes is valid iter.
        auto remove_impl(List_t::iterator child_iter)
            -> std::unique_ptr<Widget>;

        template <typename Widget_t, typename Iter>
        static auto make_deref_and_cast_iter(Iter i)
        {
            return Map_iterator{Map_iterator{i, deref}, cast<Widget_t>};
        }

        template <typename Widget_t, typename Iter>
        static auto make_const_deref_and_cast_iter(Iter i)
        {
            return Map_iterator{Map_iterator{i, deref_const},
                                cast_const<Widget_t>};
        }

        /// Return iterator to \p target in child_list_, or end if not found.
        auto find_impl(Widget const* target) const -> List_t::const_iterator
        {
            auto const end       = std::cend(child_list_);
            auto const begin     = std::cbegin(child_list_);
            auto const is_target = [target](auto const& w) -> bool {
                return w.get() == target;
            };
            return std::find_if(begin, end, is_target);
        }

        /// Return iterator to \p target in child_list_, or end if not found.
        auto find_impl(Widget const* target) -> List_t::iterator
        {
            auto const end       = std::end(child_list_);
            auto const begin     = std::begin(child_list_);
            auto const is_target = [target](auto const& w) -> bool {
                return w.get() == target;
            };
            return std::find_if(begin, end, is_target);
        }
    };

   protected:
    Children children_{this};

   private:
    std::string name_;
    std::uint16_t const unique_id_;
    Widget* parent_              = nullptr;
    bool enabled_                = false;
    bool brush_paints_wallpaper_ = true;
    std::optional<Glyph> wallpaper_;

    detail::Screen_descriptor screen_state_;
    std::set<Widget*> event_filters_;

    // Top left point of *this, relative to the top left of the screen.
    // This Point is the same with or without a border enabled.
    Point top_left_position_{0uL, 0uL};

    // The entire area of the widget, including any border space.
    Area outer_area_{width_policy.hint(), height_policy.hint()};

   public:
    /// Should only be used by Move_event send() function.
    void set_top_left(Point p) { top_left_position_ = p; }

    /// Should only be used by Resize_event send() function.
    void set_outer_area(Area a) { outer_area_ = a; }

    void set_parent(Widget* parent) { parent_ = parent; }
};

/// Helper function to create an instance.
template <typename... Args>
auto widget(Args&&... args) -> std::unique_ptr<Widget>
{
    return std::make_unique<Widget>(std::forward<Args>(args)...);
}

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGET_HPP
