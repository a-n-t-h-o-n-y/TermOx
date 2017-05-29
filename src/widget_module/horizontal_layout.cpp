#include "widget_module/layouts/horizontal_layout.hpp"

#include "painter_module/geometry.hpp"
#include "system_module/events/move_event.hpp"
#include "system_module/events/resize_event.hpp"
#include "system_module/system.hpp"
#include "widget_module/size_policy.hpp"
#include "widget_module/border.hpp"
#include <cstddef>
#include <deque>
#include <functional>
#include <tuple>
#include <vector>

namespace cppurses {

std::vector<std::size_t> Horizontal_layout::size_widgets() {
    // <Widget*, width, height>
    std::vector<std::tuple<Widget*, std::size_t, std::size_t>> widgets;
    std::size_t total_stretch{0};
    for (Object* c : this->children()) {
        Widget* w{dynamic_cast<Widget*>(c)};
        if (w != nullptr) {
            widgets.emplace_back(w, 0, 0);
            total_stretch += w->geometry().size_policy().horizontal_stretch;
        }
    }

    int width_available = this->width();

    // HORIZONTAL
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
        too_small_ = true;
        return std::vector<std::size_t>();
    }

    // Set Size_policy::Ignored widgets to their stretch factor width value
    for (auto& tup : widgets) {
        if (std::get<0>(tup)->geometry().size_policy().horizontal_policy ==
            Size_policy::Ignored) {
            const float percent =
                std::get<0>(tup)->geometry().size_policy().horizontal_stretch /
                static_cast<float>(total_stretch);
            std::size_t width = percent * this->width();
            if (width < std::get<0>(tup)->geometry().min_width()) {
                width = std::get<0>(tup)->geometry().min_width();
            } else if (width > std::get<0>(tup)->geometry().max_width()) {
                width = std::get<0>(tup)->geometry().max_width();
            }
            std::get<1>(tup) = width;
            width_available -= width;
        }
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
    std::vector<std::tuple<Widget*, std::reference_wrapper<std::size_t>,
                           std::reference_wrapper<std::size_t>>>
        widgets_w_refs;
    for (auto& tup : widgets) {
        widgets_w_refs.emplace_back(std::get<0>(tup), std::get<1>(tup),
                                    std::get<2>(tup));
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
    for (auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Fixed) {
            std::get<2>(tup) = std::get<0>(tup)->geometry().height_hint();
            if (std::get<2>(tup) > this->height()) {
                too_small_ = true;
                return std::vector<std::size_t>();
            }
        } else if (policy == Size_policy::Ignored ||
                   policy == Size_policy::Preferred ||
                   policy == Size_policy::Expanding) {
            std::get<2>(tup) = this->height();
            if (std::get<2>(tup) > std::get<0>(tup)->geometry().max_height()) {
                std::get<2>(tup) = std::get<0>(tup)->geometry().max_height();
            } else if (std::get<2>(tup) <
                       std::get<0>(tup)->geometry().min_height()) {
                too_small_ = true;
                return std::vector<std::size_t>();
            }
        } else if (policy == Size_policy::Maximum) {
            std::get<2>(tup) = this->height();
            if (std::get<2>(tup) > std::get<0>(tup)->geometry().height_hint()) {
                std::get<2>(tup) = std::get<0>(tup)->geometry().height_hint();
            } else if (std::get<2>(tup) <
                       std::get<0>(tup)->geometry().min_height()) {
                too_small_ = true;
                return std::vector<std::size_t>();
            }
        } else if (policy == Size_policy::Minimum ||
                   policy == Size_policy::MinimumExpanding) {
            std::get<2>(tup) = this->height();
            if (std::get<2>(tup) < std::get<0>(tup)->geometry().height_hint()) {
                std::get<2>(tup) = std::get<0>(tup)->geometry().height_hint();
            }
            if (std::get<2>(tup) > std::get<0>(tup)->geometry().max_height() ||
                std::get<2>(tup) > this->height()) {
                too_small_ = true;
                return std::vector<std::size_t>();
            }
        }
    }

    // Post all Resize_events
    for (auto& tup : widgets) {
        System::post_event(
            std::get<0>(tup),
            std::make_unique<Resize_event>(std::get<1>(tup), std::get<2>(tup)));
    }
    std::vector<std::size_t> widths;
    widths.reserve(widgets.size());
    for (const auto& tup : widgets) {
        widths.push_back(std::get<1>(tup));
    }
    return widths;
}

void Horizontal_layout::distribute_space(
    std::vector<std::tuple<Widget*,
                           std::reference_wrapper<std::size_t>,
                           std::reference_wrapper<std::size_t>>> widgets,
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
    std::deque<std::size_t> width_additions;
    int index{0};
    auto to_distribute = width_left;
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Expanding ||
            policy == Size_policy::MinimumExpanding) {
            width_additions.push_back(
                (std::get<0>(tup)->geometry().size_policy().horizontal_stretch /
                 static_cast<double>(total_stretch)) *
                to_distribute);
            if ((std::get<1>(tup).get() + width_additions.back()) >
                std::get<0>(tup)->geometry().max_width()) {
                width_left -=
                    std::get<0>(tup)->geometry().max_width() - std::get<1>(tup);
                std::get<1>(tup).get() =
                    std::get<0>(tup)->geometry().max_width();
                // width_left -= std::get<1>(tup);
                widgets.erase(std::begin(widgets) + index);
                return distribute_space(widgets, width_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign values
    for (auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Expanding ||
            policy == Size_policy::MinimumExpanding) {
            std::get<1>(tup).get() += width_additions.front();
            width_left -= width_additions.front();
            width_additions.pop_front();
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
            policy == Size_policy::Minimum || policy == Size_policy::Ignored) {
            total_stretch +=
                std::get<0>(tup)->geometry().size_policy().horizontal_stretch;
        }
    }

    // Calculate new widths of widgets in new group, if any go over max_width
    // then assign max value and recurse without that widget in vector.
    width_additions.clear();
    index = 0;
    to_distribute = width_left;
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Preferred ||
            policy == Size_policy::Minimum || policy == Size_policy::Ignored) {
            width_additions.push_back(
                (std::get<0>(tup)->geometry().size_policy().horizontal_stretch /
                 static_cast<double>(total_stretch)) *
                to_distribute);
            if ((std::get<1>(tup).get() + width_additions.back()) >
                std::get<0>(tup)->geometry().max_width()) {
                width_left -=
                    std::get<0>(tup)->geometry().max_width() - std::get<1>(tup);
                std::get<1>(tup).get() =
                    std::get<0>(tup)->geometry().max_width();
                // width_left -= std::get<1>(tup);
                widgets.erase(std::begin(widgets) + index);
                return distribute_space(widgets, width_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign values
    for (auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Preferred ||
            policy == Size_policy::Minimum || policy == Size_policy::Ignored) {
            std::get<1>(tup).get() += width_additions.front();
            width_left -= width_additions.front();
            width_additions.pop_front();
        }
    }

    if (width_left == 0) {
        return;
    }
    // Rounding error extra
    // First Group
    auto width_check{0};
    do {
        width_check = width_left;
        for (auto& tup : widgets) {
            auto policy =
                std::get<0>(tup)->geometry().size_policy().horizontal_policy;
            if ((policy == Size_policy::Expanding ||
                 policy == Size_policy::MinimumExpanding) &&
                width_left > 0) {
                if (std::get<1>(tup).get() + 1 <=
                    std::get<0>(tup)->geometry().max_width()) {
                    std::get<1>(tup).get() += 1;
                    width_left -= 1;
                }
            }
        }
    } while (width_check != width_left);

    // Second Group
    do {
        width_check = width_left;
        for (auto& tup : widgets) {
            auto policy =
                std::get<0>(tup)->geometry().size_policy().horizontal_policy;
            if ((policy == Size_policy::Preferred ||
                 policy == Size_policy::Minimum ||
                 policy == Size_policy::Ignored) &&
                width_left > 0) {
                if (std::get<1>(tup).get() + 1 <=
                    std::get<0>(tup)->geometry().max_width()) {
                    std::get<1>(tup).get() += 1;
                    width_left -= 1;
                }
            }
        }
    } while (width_check != width_left);
}

void Horizontal_layout::collect_space(
    std::vector<std::tuple<Widget*,
                           std::reference_wrapper<std::size_t>,
                           std::reference_wrapper<std::size_t>>> widgets,
    int width_left) {
    if (width_left == 0) {
        return;
    }
    // Find total stretch of first group
    std::size_t total_stretch{0};
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Ignored) {
            total_stretch +=
                std::get<0>(tup)->geometry().size_policy().horizontal_stretch;
        }
    }

    // Find total of inverse of percentages
    double total_inverse{0};
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Ignored) {
            total_inverse +=
                1 /
                (std::get<0>(tup)->geometry().size_policy().horizontal_stretch /
                 static_cast<double>(total_stretch));
        }
    }

    // Calculate new widths of widgets in new group, if any go under min_width
    // then assign min value and recurse without that widget in vector.
    std::deque<std::size_t> width_deductions;
    int index{0};
    auto to_collect = width_left;
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Ignored) {
            width_deductions.push_back(
                ((1 / (std::get<0>(tup)
                           ->geometry()
                           .size_policy()
                           .horizontal_stretch /
                       static_cast<double>(total_stretch))) /
                 static_cast<double>(total_inverse)) *
                (to_collect * -1));
            if ((std::get<1>(tup).get() - width_deductions.back()) <
                std::get<0>(tup)->geometry().min_width()) {
                width_left += std::get<1>(tup).get() -
                              std::get<0>(tup)->geometry().min_width();
                std::get<1>(tup).get() =
                    std::get<0>(tup)->geometry().min_width();
                widgets.erase(std::begin(widgets) + index);
                return collect_space(widgets, width_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign calculated
    // values
    for (auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Ignored) {
            if (std::get<1>(tup).get() >= width_deductions.front()) {
                std::get<1>(tup).get() -= width_deductions.front();
            } else {
                std::get<1>(tup).get() = 0;
            }
            width_left += width_deductions.front();
            width_deductions.pop_front();
        }
    }

    // SECOND GROUP - duplicate of above dependent on Policies to work with.
    if (width_left == 0) {
        return;
    }
    // Find total stretch
    total_stretch = 0;
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Expanding) {
            total_stretch +=
                std::get<0>(tup)->geometry().size_policy().horizontal_stretch;
        }
    }

    // Find total of inverse of percentages
    total_inverse = 0;
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Expanding) {
            total_inverse +=
                1 /
                (std::get<0>(tup)->geometry().size_policy().horizontal_stretch /
                 static_cast<double>(total_stretch));
        }
    }

    // Calculate new widths of widgets in new group, if any go over max_width
    // then assign max value and recurse without that widget in vector.
    width_deductions.clear();
    index = 0;
    to_collect = width_left;
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Expanding) {
            width_deductions.push_back(
                ((1 / (std::get<0>(tup)
                           ->geometry()
                           .size_policy()
                           .horizontal_stretch /
                       static_cast<double>(total_stretch))) /
                 static_cast<double>(total_inverse)) *
                (to_collect * -1));
            if ((std::get<1>(tup).get() - width_deductions.back()) <
                std::get<0>(tup)->geometry().min_width()) {
                width_left += std::get<1>(tup).get() -
                              std::get<0>(tup)->geometry().min_width();
                std::get<1>(tup).get() =
                    std::get<0>(tup)->geometry().min_width();
                widgets.erase(std::begin(widgets) + index);
                return collect_space(widgets, width_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign calculated
    // values
    for (auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Expanding) {
            if (std::get<1>(tup).get() >= width_deductions.front()) {
                std::get<1>(tup).get() -= width_deductions.front();
            } else {
                std::get<1>(tup).get() = 0;
            }
            width_left += width_deductions.front();
            width_deductions.pop_front();
        }
    }
    // Change this to distribute the space, it might not be too small
    if (width_left != 0) {
        too_small_ = true;
        return;
    }
}

void Horizontal_layout::position_widgets(
    const std::vector<std::size_t>& widths) {
    std::vector<Widget*> widgets;
    for (Object* c : this->children()) {
        Widget* w{dynamic_cast<Widget*>(c)};
        if (w != nullptr) {
            widgets.push_back(w);
        }
    }
    if (widgets.size() != widths.size()) {
        return;
    }
    std::size_t x_pos{west_border_offset(this->border())};
    std::size_t index{0};
    for (auto& widg : widgets) {
        System::post_event(
            widg, std::make_unique<Move_event>(
                      this->x() + x_pos,
                      this->y() + north_border_offset(this->border())));
        x_pos += widths.at(index++);
    }
}

void Horizontal_layout::update_geometry() {
    auto widths = this->size_widgets();
    this->position_widgets(widths);
}

}  // namespace cppurses
