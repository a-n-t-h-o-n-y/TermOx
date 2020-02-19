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
    Get_policy_t get_policy;  // Size_policy const&(Widget const&)
    Get_length_t get_length;  // std::size_t(Widget const&)
    Get_offset_t get_offset;  // std::size_t(Widget const&)
};

template <typename Primary_t,
          typename Secondary_t,
          typename Get_area_t,
          typename Get_point_t>
struct Linear_layout_parameters {
    Primary_t primary;
    Secondary_t secondary;
    Get_area_t get_area;    // Area(std::size_t primary, std::size_t secondary)
    Get_point_t get_point;  // Point(std::size_t primary, std::size_t secondary)
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

   private:
    inline static auto const parameters_ = Parameters{};

    Shared_space<Parameters> shared_space_;
    Unique_space<Parameters> unique_space_;

    using Length_list   = std::vector<std::size_t>;
    using Position_list = std::vector<std::size_t>;

   private:
    void send_enable_disable_events(Length_list const& primary,
                                    Length_list const& secondary)
    {
        /* TODO temporary */
        if (primary.size() != secondary.size())
            throw std::logic_error{"Linear_layout::send_...events"};
        auto const children = this->get_children();
        if (children.size() != primary.size())
            throw std::logic_error{"Linear_layout::send_...events"};
        /* TODO temporary */

        for (auto i = 0uL; i < primary.size(); ++i) {
            auto& child = children[i];
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
            if (children[i].enabled()) {
                auto const area =
                    parameters_.get_area(primary[i], secondary[i]);
                System::post_event<Resize_event>(children[i], area);
            }
        }
    }

    void send_move_events(Position_list const& primary,
                          Position_list const& secondary)
    {
        auto const children         = this->get_children();
        auto const primary_offset   = parameters_.primary.get_offset(*this);
        auto const secondary_offset = parameters_.secondary.get_offset(*this);
        for (auto i = 0uL; i < primary.size(); ++i) {
            if (children[i].enabled()) {
                auto const point =
                    parameters_.get_point(primary[i] + primary_offset,
                                          secondary[i] + secondary_offset);
                System::post_event<Move_event>(children[i], point);
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
