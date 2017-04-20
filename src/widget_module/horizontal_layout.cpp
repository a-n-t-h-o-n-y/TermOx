#include <widget_module/layouts/horizontal_layout.hpp>

#include <system_module/events/move_event.hpp>
#include <system_module/events/resize_event.hpp>
#include <system_module/system.hpp>
#include <widget_module/border.hpp>
#include <widget_module/size_policy.hpp>
#include <painter_module/geometry.hpp>

#include <cstddef>
#include <vector>
#include <deque>
#include <tuple>

namespace twf {

void Horizontal_layout::size_widgets() {
    // <Widget*, width, height>
    std::vector<std::tuple<Widget*, std::size_t, std::size_t>> widgets;
    std::size_t total_stretch{0};
    for (Object* c : this->children()) {
        Widget* w{dynamic_cast<Widget*>(c)};
        if (w != nullptr) {
            widgets.push_back(std::make_tuple(w, 0, 0));
            total_stretch += w->geometry().size_policy().horizontal_stretch;
        }
    }

    int width_available = this->width();

    // HORIZONTAL
    // Set Size_policy::Ignored widgets to their stretch factor width value
    for (auto& tup : widgets) {
        if (std::get<0>(tup)->geometry().size_policy().horizontal_policy ==
            Size_policy::Ignored) {
            const float percent =
                std::get<0>(tup)->geometry().size_policy().horizontal_stretch /
                static_cast<float>(total_stretch);
            std::size_t width = percent * width_available;
            if (width < std::get<0>(tup)->geometry().min_width()) {
                width = std::get<0>(tup)->geometry().min_width();
            } else if (width > std::get<0>(tup)->geometry().max_width()) {
                width = std::get<0>(tup)->geometry().max_width();
            }
            std::get<1>(tup) = width;
            width_available -= width;
        }
    }

    // Set Fixed, Minimum and MinimumExpanding to width_hint
    for (auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Fixed || policy == Size_policy::Minimum ||
            policy == Size_policy::MinimumExpanding) {
            std::get<1>(tup) = std::get<0>(tup)->geometry().width_hint();
            width_available -= std::get<1>(tup);
        }
    }
    if (width_available < 0) {
        this->paint_too_small_warning();
    }

    // Set Maximum, Preferred and Expanding to width_hint
    for (auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Expanding) {
            std::get<1>(tup) = std::get<0>(tup)->geometry().width_hint();
            width_available -= std::get<1>(tup);
        }
    }

    // create vector of size references for below if statements
    std::vector<std::tuple<Widget*, std::size_t&, std::size_t&>> widgets_w_refs;
    for (auto& tup : widgets) {
        widgets_w_refs.push_back(
            std::tie(std::get<0>(tup), std::get<1>(tup), std::get<2>(tup)));
    }
    // If space left, fill in expanding and min_expanding, then if still,
    // preferred and min
    if (width_available > 0) {
        this->distribute_space(widgets_w_refs, width_available);
    }

    // if negative space left, subtract from max and preferred, then if still
    // needed, expanding
    if (width_available < 0) {
        this->collect_space(widgets_w_refs, width_available);
    }

    // VERTICAL - repeat the above, but with vertical properties

    // Post all Resize_events
    for (auto& tup : widgets) {
        System::post_event(
            std::get<0>(tup),
            std::make_unique<Resize_event>(std::get<1>(tup), std::get<2>(tup)));
    }
}

void Horizontal_layout::position_widgets() {}

void Horizontal_layout::update_geometry() {
    this->size_widgets();
    this->position_widgets();
}

void Horizontal_layout::distribute_space(
    std::vector<std::tuple<Widget*, std::size_t&, std::size_t&>> widgets,
    int width_left) {
    // Find total stretch of first group
    std::size_t total_stretch{0};
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Expanding ||
            policy == Size_policy::MinimumExpanding) {
            total_stretch +=
                std::get<0>(tup)->geometry().size_policy().horizontal_stretch;
        }
    }

    // Calculate new widths of widgets in new group, if any go over max_width
    // then assign max value and recurse without that widget in vector.
    std::deque<std::size_t> possible_widths;
    int index{0};
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Expanding ||
            policy == Size_policy::MinimumExpanding) {
            possible_widths.push_back(
                (std::get<0>(tup)->geometry().size_policy().horizontal_stretch /
                 static_cast<double>(total_stretch)) *
                width_left);
            if ((std::get<1>(tup) + possible_widths.back()) >
                std::get<0>(tup)->geometry().max_width()) {
                std::get<1>(tup) = std::get<0>(tup)->geometry().max_width();
                width_left -= std::get<1>(tup);
                widgets.erase(std::begin(widgets) + index);
                return distribute_space(widgets, width_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign calculated
    // values
    for (auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Expanding ||
            policy == Size_policy::MinimumExpanding) {
            std::get<1>(tup) = possible_widths.front();
            width_left -= possible_widths.front();
            possible_widths.pop_front();
        }
    }

    // SECOND GROUP - duplicate of above dependent on Policies to work with.
    // Preferred and Minimum
    if (width_left == 0) {
        return;
    }
    // Find total stretch
    total_stretch = 0;
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Preferred ||
            policy == Size_policy::Minimum) {
            total_stretch +=
                std::get<0>(tup)->geometry().size_policy().horizontal_stretch;
        }
    }

    // Calculate new widths of widgets in new group, if any go over max_width
    // then assign max value and recurse without that widget in vector.
    possible_widths.clear();
    index = 0;
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Preferred ||
            policy == Size_policy::Minimum) {
            possible_widths.push_back(
                (std::get<0>(tup)->geometry().size_policy().horizontal_stretch /
                 static_cast<double>(total_stretch)) *
                width_left);
            if ((std::get<1>(tup) + possible_widths.back()) >
                std::get<0>(tup)->geometry().max_width()) {
                std::get<1>(tup) = std::get<0>(tup)->geometry().max_width();
                width_left -= std::get<1>(tup);
                widgets.erase(std::begin(widgets) + index);
                return distribute_space(widgets, width_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign calculated
    // values
    for (auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Preferred ||
            policy == Size_policy::Minimum) {
            std::get<1>(tup) = possible_widths.front();
            width_left -= possible_widths.front();
            possible_widths.pop_front();
        }
    }
}

void Horizontal_layout::collect_space(
    std::vector<std::tuple<Widget*, std::size_t&, std::size_t&>> widgets,
    int width_left) {
    //
}

// void Horizontal_layout::update_geometry() {
//     std::size_t border_space{0};
//     std::size_t total_stretch{0};
//     for (const Object* c : this->children()) {
//         const Widget* cw{dynamic_cast<const Widget*>(c)};
//         if (cw != nullptr) {
//             // Border space
//             if ((cw->border().west_enabled() ||
//                  cw->border().north_west_enabled() ||
//                  cw->border().south_west_enabled()) &&
//                 cw->border().enabled()) {
//                 ++border_space;
//             }
//             if ((cw->border().east_enabled() ||
//                  cw->border().north_east_enabled() ||
//                  cw->border().south_east_enabled()) &&
//                 cw->border().enabled()) {
//                 ++border_space;
//             }
//             // Stretch factor
//             total_stretch += cw->size_policy().horizontal_stretch;
//         }
//     }
//     std::size_t widg_space{0};
//     if (this->geometry().width() < border_space) {
//         widg_space = 0;
//     } else {
//         widg_space = this->geometry().width() - border_space;
//     }
//     // std::size_t widg_space = this->geometry().width() - border_space;
//     std::size_t x_pos{0};
//     for (Object* c : this->children()) {
//         Widget* cw{dynamic_cast<Widget*>(c)};
//         if (cw != nullptr) {
//             // Position
//             if ((cw->border().west_enabled() ||
//                  cw->border().north_west_enabled() ||
//                  cw->border().south_west_enabled()) &&
//                 cw->border().enabled()) {
//                 ++x_pos;
//             }
//             std::size_t y_pos{0};
//             if ((cw->border().north_enabled() ||
//                  cw->border().north_west_enabled() ||
//                  cw->border().north_east_enabled()) &&
//                 cw->border().enabled()) {
//                 ++y_pos;
//             }
//             // Size
//             if (total_stretch == 0) {
//                 total_stretch = this->children().size();
//             }
//             std::size_t width =
//                 widg_space *
//                 (cw->size_policy().horizontal_stretch /
//                 double(total_stretch));
//             std::size_t height{this->geometry().height()};
//             if ((cw->border().north_enabled() ||
//                  cw->border().north_west_enabled() ||
//                  cw->border().north_east_enabled()) &&
//                 cw->border().enabled() && height != 0) {
//                 --height;
//             }
//             if ((cw->border().south_enabled() ||
//                  cw->border().south_west_enabled() ||
//                  cw->border().south_east_enabled()) &&
//                 cw->border().enabled() && height != 0) {
//                 --height;
//             }
//             System::post_event(cw,
//                                std::make_unique<Resize_event>(width,
//                                height));
//             System::post_event(cw, std::make_unique<Move_event>(
//                                        this->x() + x_pos, this->y() +
//                                        y_pos));
//             x_pos += width;
//             if ((cw->border().east_enabled() ||
//                  cw->border().north_east_enabled() ||
//                  cw->border().south_east_enabled()) &&
//                 cw->border().enabled()) {
//                 ++x_pos;
//             }
//             // if last widget, extend to rest of layout
//             if (c == *(--std::end(this->children())) &&
//                 this->geometry().width() > x_pos) {
//                 System::post_event(
//                     cw, std::make_unique<Resize_event>(
//                             width + this->geometry().width() - x_pos,
//                             height));
//             }
//         }
//     }
// }

}  // namespace twf
