#ifndef CPPURSES_WIDGET_LAYOUTS_DETAIL_LINEAR_LAYOUT_HPP
#define CPPURSES_WIDGET_LAYOUTS_DETAIL_LINEAR_LAYOUT_HPP
#include <cppurses/system/events/move_event.hpp>
#include <cppurses/system/events/resize_event.hpp>
#include <cppurses/widget/layout.hpp>

#include "shared_space.hpp"
#include "unique_space.hpp"

namespace cppurses::layout::detail {

template <typename Get_policy_t, typename Get_length_t, typename Get_offset_t>
struct Dimension_parameters {
    using get_policy = Get_policy_t;  // Size_policy const&(Widget const&)
    using get_length = Get_length_t;  // std::size_t(Widget const&) [w/h]
    using get_offset = Get_offset_t;  // std::size_t(Widget const&) [global x/y]
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
template <typename Child_t, typename Parameters>
class Linear_layout : public Layout<Child_t> {
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

    /// Return the child Widget offset, the first widget included in the layout.
    auto get_offset() const -> std::size_t { return offset_; }

    /// Sets the child Widget offset, does not do bounds checking.
    auto set_offset(std::size_t index)
    {
        offset_ = index;
        shared_space_.set_offset(index);
        unique_space_.set_offset(index);
        this->update_geometry();
    }

   private:
    using Length_list   = std::vector<std::size_t>;
    using Position_list = std::vector<std::size_t>;

    std::size_t offset_ = 0uL;

    Shared_space<Parameters> shared_space_;
    Unique_space<Parameters> unique_space_;

   private:
    void send_enable_disable_events(Length_list const& primary,
                                    Length_list const& secondary)
    {
        auto const children = this->get_children();
        for (auto i = 0uL; i < offset_; ++i) {
            if (children[i].enabled())
                children[i].disable(true, false);
        }
        for (auto i = 0uL; i < primary.size(); ++i) {
            auto& child = children[offset_ + i];
            if (is_valid(primary[i], secondary[i])) {
                if (not child.enabled())
                    child.enable(true, false);
            }
            else {
                if (child.enabled())
                    child.disable(true, false);
            }
        }
    }

    void send_resize_events(Length_list const& primary,
                            Length_list const& secondary)
    {
        auto const children = this->get_children();
        for (auto i = 0uL; i < primary.size(); ++i) {
            auto& child = children[offset_ + i];
            if (child.enabled()) {
                auto const area =
                    typename Parameters::get_area{}(primary[i], secondary[i]);
                System::post_event<Resize_event>(child, area);
            }
        }
    }

    void send_move_events(Position_list const& primary,
                          Position_list const& secondary)
    {
        auto const children = this->get_children();
        auto const primary_offset =
            typename Parameters::Primary::get_offset{}(*this);
        auto const secondary_offset =
            typename Parameters::Secondary::get_offset{}(*this);
        for (auto i = 0uL; i < primary.size(); ++i) {
            auto& child = children[offset_ + i];
            if (child.enabled()) {
                auto const point = typename Parameters::get_point{}(
                    primary[i] + primary_offset,
                    secondary[i] + secondary_offset);
                System::post_event<Move_event>(child, point);
            }
        }
    }

    /// Determine whether a widget is valid to display from its lengths
    /** A length of zero in any dimension means the Widget will be disabled. */
    static auto is_valid(std::size_t primary, std::size_t secondary) -> bool
    {
        return primary != 0 && secondary != 0;
    }
};

}  // namespace cppurses::layout::detail
#endif  // CPPURSES_WIDGET_LAYOUTS_DETAIL_LINEAR_LAYOUT_HPP
