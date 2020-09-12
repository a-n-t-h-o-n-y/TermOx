#ifndef CPPURSES_WIDGET_LAYOUTS_DETAIL_LINEAR_LAYOUT_HPP
#define CPPURSES_WIDGET_LAYOUTS_DETAIL_LINEAR_LAYOUT_HPP
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/widget/layout.hpp>

#include "shared_space.hpp"
#include "unique_space.hpp"

namespace cppurses::layout::detail {

template <typename Get_policy_t,
          typename Get_length_t,
          typename Get_offset_t,
          Policy_direction d>
struct Dimension_parameters {
    using get_policy = Get_policy_t;  // Size_policy const&(Widget const&)
    using get_length = Get_length_t;  // std::size_t(Widget const&) [w/h]
    using get_offset = Get_offset_t;  // std::size_t(Widget const&) [global x/y]
    static auto const direction = d;
};

template <typename Primary_t,
          typename Secondary_t,
          typename Get_area_t,
          typename Get_point_t>
struct Linear_layout_parameters {
    using Primary   = Primary_t;
    using Secondary = Secondary_t;
    using get_area  = Get_area_t;   // Area(size_t primary, size_t secondary)
    using get_point = Get_point_t;  // Point(size_t primary, size_t secondary)
};

/// Lays out Widgets in 2D, sharing space in a primary dimension.
/** The secondary dimension does not share space among Widgets. */
template <typename Child, typename Parameters>
class Linear_layout : public Layout<Child> {
   public:
    using Child_t = Child;

   private:
    using Base_t = Layout<Child_t>;

   public:
    using Layout<Child>::Layout;

   public:
    /// Erase first element that satisfies \p pred. Return true if erase happens
    template <typename Unary_predicate_t,
              std::enable_if_t<
                  std::is_invocable_v<Unary_predicate_t,
                                      std::add_lvalue_reference_t<Child_t>>,
                  int> = 0>
    auto erase(Unary_predicate_t&& pred) -> bool
    {
        auto r = this->Base_t::erase(std::forward<Unary_predicate_t>(pred));
        this->reset_offset_if_out_of_bounds();
        return r;
    }

    void erase(Widget const* child)
    {
        this->Base_t::children_.erase(child);
        this->reset_offset_if_out_of_bounds();
    }

    void erase(std::size_t index)
    {
        this->Base_t::children_.erase(index);
        this->reset_offset_if_out_of_bounds();
    }

    /// Sets the child Widget offset, does not do bounds checking.
    void set_offset(std::size_t index)
    {
        this->Widget::children_.set_offset(index);
        shared_space_.set_offset(index);
        unique_space_.set_offset(index);
        System::post_event(Child_polished_event{*this, *this});
        this->force_repaint_empty_space();
    }

    void decrement_offset()
    {
        if (this->Widget::child_offset() == 0uL)
            return;
        this->set_offset(this->Widget::child_offset() - 1uL);
    }

    void increment_offset()
    {
        if (this->Widget::child_offset() + 1uL >= this->child_count())
            return;
        this->set_offset(this->Widget::child_offset() + 1uL);
    }

   protected:
    using Parameters_t = Parameters;

   protected:
    void update_geometry() override
    {
        auto const primary_lengths = shared_space_.calculate_lengths(*this);
        auto const primary_pos =
            shared_space_.calculate_positions(primary_lengths);

        auto const secondary_lengths = unique_space_.calculate_lengths(*this);
        auto const secondary_pos =
            unique_space_.calculate_positions(secondary_lengths);

        this->send_enable_disable_events(primary_lengths, secondary_lengths);
        this->send_resize_events(primary_lengths, secondary_lengths);
        this->send_move_events(primary_pos, secondary_pos);
    }

    auto child_polished_event(Widget& child) -> bool override
    {
        if (this->width_policy.is_passive()) {
            Widget* parent = this->parent();
            // Find first parent that is not passive.
            while (parent != nullptr && parent->width_policy.is_passive())
                parent = parent->parent();
            if (parent != nullptr)
                System::post_event(Child_polished_event{*parent, *this});
        }
        if (this->height_policy.is_passive()) {
            Widget* parent = this->parent();
            // Find first parent that is not passive.
            while (parent != nullptr && parent->height_policy.is_passive())
                parent = parent->parent();
            if (parent != nullptr)
                System::post_event(Child_polished_event{*parent, *this});
        }
        this->update_geometry();
        return Widget::child_polished_event(child);
    }

   private:
    using Length_list   = std::vector<std::size_t>;
    using Position_list = std::vector<std::size_t>;

    Shared_space<Parameters> shared_space_;
    Unique_space<Parameters> unique_space_;

   private:
    // Hack until the paint system is updated and treats layouts accordingly.
    void force_repaint_empty_space() { Painter{*this}; }

    void send_enable_disable_events(Length_list const& primary,
                                    Length_list const& secondary)
    {
        auto const children = this->Widget::get_children();
        auto const offset   = this->Widget::child_offset();
        for (auto i = 0uL; i < offset; ++i) {
            if (children[i].is_enabled())
                children[i].disable(true, false);
        }
        for (auto i = 0uL; i < primary.size(); ++i) {
            auto& child = children[offset + i];
            if (is_valid(primary[i], secondary[i])) {
                if (not child.is_enabled())
                    child.enable(true, false);
            }
            else {
                if (child.is_enabled())
                    child.disable(true, false);
            }
        }
    }

    void send_resize_events(Length_list const& primary,
                            Length_list const& secondary)
    {
        auto const children = this->Widget::get_children();
        auto const offset   = this->Widget::child_offset();
        for (auto i = 0uL; i < primary.size(); ++i) {
            auto& child = children[offset + i];
            if (child.is_enabled()) {
                auto const area =
                    typename Parameters::get_area{}(primary[i], secondary[i]);
                System::post_event(Resize_event{child, area});
            }
        }
    }

    void send_move_events(Position_list const& primary,
                          Position_list const& secondary)
    {
        auto const children = this->Widget::get_children();
        auto const offset   = this->Widget::child_offset();
        auto const primary_offset =
            typename Parameters::Primary::get_offset{}(*this);
        auto const secondary_offset =
            typename Parameters::Secondary::get_offset{}(*this);
        for (auto i = 0uL; i < primary.size(); ++i) {
            auto& child = children[offset + i];
            if (child.is_enabled()) {
                auto const point = typename Parameters::get_point{}(
                    primary[i] + primary_offset,
                    secondary[i] + secondary_offset);
                System::post_event(Move_event{child, point});
            }
        }
    }

    /// Determine whether a widget is valid to display from its lengths
    /** A length of zero in any dimension means the Widget will be disabled. */
    // TODO This is ambiguous, what is a zero width/height wants to be enabled?
    // Need another data member to tell if something should be enabled or not.
    static auto is_valid(std::size_t primary, std::size_t secondary) -> bool
    {
        return primary != 0 && secondary != 0;
    }

    void reset_offset_if_out_of_bounds()
    {
        auto const count = this->child_count();
        if (count != 0 && this->children_.get_offset() >= count)
            this->set_offset(count - 1);
    }
};

}  // namespace cppurses::layout::detail
#endif  // CPPURSES_WIDGET_LAYOUTS_DETAIL_LINEAR_LAYOUT_HPP
