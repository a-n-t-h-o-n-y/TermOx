#include "widget/layouts/vertical_layout.hpp"

#include "painter/geometry.hpp"
#include "system/events/move_event.hpp"
#include "system/events/resize_event.hpp"
#include "system/system.hpp"
#include "widget/border.hpp"
#include "widget/size_policy.hpp"
#include <cstddef>
#include <deque>
#include <functional>
#include <tuple>
#include <vector>

namespace cppurses {

std::vector<std::size_t> Vertical_layout::size_widgets() {
    // <Widget*, width, height>
    std::vector<std::tuple<Widget*, std::size_t, std::size_t>> widgets;
    std::size_t total_stretch{0};
    for (Widget* c : this->children()) {
        widgets.emplace_back(std::make_tuple(c, 0, 0));
        total_stretch += c->geometry().size_policy().vertical_stretch;
    }

    int height_available = this->height();

    // VERTICAL
    // Set Fixed, Minimum and MinimumExpanding to height_hint
    for (auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Fixed || policy == Size_policy::Minimum ||
            policy == Size_policy::MinimumExpanding) {
            std::get<2>(tup) = std::get<0>(tup)->geometry().height_hint();
            height_available -= std::get<2>(tup);
        }
    }
    if (height_available < 0) {
        too_small_ = true;
        return std::vector<std::size_t>();
    }

    // Set Size_policy::Ignored widgets to their stretch factor height value
    for (auto& tup : widgets) {
        if (std::get<0>(tup)->geometry().size_policy().vertical_policy ==
            Size_policy::Ignored) {
            const float percent =
                std::get<0>(tup)->geometry().size_policy().vertical_stretch /
                static_cast<float>(total_stretch);
            std::size_t height = percent * this->height();
            if (height < std::get<0>(tup)->geometry().min_height()) {
                height = std::get<0>(tup)->geometry().min_height();
            } else if (height > std::get<0>(tup)->geometry().max_height()) {
                height = std::get<0>(tup)->geometry().max_height();
            }
            std::get<2>(tup) = height;
            height_available -= height;
        }
    }

    // Set Maximum, Preferred and Expanding to height_hint
    for (auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Expanding) {
            std::get<2>(tup) = std::get<0>(tup)->geometry().height_hint();
            height_available -= std::get<2>(tup);
        }
    }

    // create vector of size references for below if statements
    std::vector<std::tuple<Widget*, std::reference_wrapper<std::size_t>,
                           std::reference_wrapper<std::size_t>>>
        widgets_w_refs;
    for (auto& tup : widgets) {
        widgets_w_refs.emplace_back(
            std::tie(std::get<0>(tup), std::get<1>(tup), std::get<2>(tup)));
    }
    // If space left, fill in expanding and min_expanding, then if still,
    // preferred and min
    if (height_available > 0) {
        this->distribute_space(widgets_w_refs, height_available);
    }

    // if negative space left, subtract from max and preferred, then if still
    // needed, expanding
    if (height_available < 0) {
        this->collect_space(widgets_w_refs, height_available);
    }

    // HORIZONTAL - repeat the above, but with horizontal properties
    for (auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().horizontal_policy;
        if (policy == Size_policy::Fixed) {
            std::get<1>(tup) = std::get<0>(tup)->geometry().width_hint();
            if (std::get<1>(tup) > this->width()) {
                too_small_ = true;
                return std::vector<std::size_t>();
            }
        } else if (policy == Size_policy::Ignored ||
                   policy == Size_policy::Preferred ||
                   policy == Size_policy::Expanding) {
            std::get<1>(tup) = this->width();
            if (std::get<1>(tup) > std::get<0>(tup)->geometry().max_width()) {
                std::get<1>(tup) = std::get<0>(tup)->geometry().max_width();
            } else if (std::get<1>(tup) <
                       std::get<0>(tup)->geometry().min_width()) {
                too_small_ = true;
                return std::vector<std::size_t>();
            }
        } else if (policy == Size_policy::Maximum) {
            std::get<1>(tup) = this->width();
            if (std::get<1>(tup) > std::get<0>(tup)->geometry().width_hint()) {
                std::get<1>(tup) = std::get<0>(tup)->geometry().width_hint();
            } else if (std::get<1>(tup) <
                       std::get<0>(tup)->geometry().min_width()) {
                too_small_ = true;
                return std::vector<std::size_t>();
            }
        } else if (policy == Size_policy::Minimum ||
                   policy == Size_policy::MinimumExpanding) {
            std::get<1>(tup) = this->width();
            if (std::get<1>(tup) < std::get<0>(tup)->geometry().width_hint()) {
                std::get<1>(tup) = std::get<0>(tup)->geometry().width_hint();
            }
            if (std::get<1>(tup) > std::get<0>(tup)->geometry().max_width() ||
                std::get<1>(tup) > this->width()) {
                too_small_ = true;
                return std::vector<std::size_t>();
            }
        }
    }

    // Post all Resize_events
    for (auto& tup : widgets) {
        System::post_event<Resize_event>(std::get<0>(tup), std::get<1>(tup),
                                         std::get<2>(tup));
    }
    std::vector<std::size_t> heights;
    heights.reserve(widgets.size());
    for (const auto& tup : widgets) {
        heights.push_back(std::get<2>(tup));
    }
    return heights;
}

void Vertical_layout::distribute_space(
    std::vector<std::tuple<Widget*,
                           std::reference_wrapper<std::size_t>,
                           std::reference_wrapper<std::size_t>>> widgets,
    int height_left) {
    // Find total stretch of first group
    std::size_t total_stretch{0};
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Expanding ||
            policy == Size_policy::MinimumExpanding) {
            total_stretch +=
                std::get<0>(tup)->geometry().size_policy().vertical_stretch;
        }
    }

    // Calculate new heights of widgets in new group, if any go over max_height
    // then assign max value and recurse without that widget in vector.
    std::deque<std::size_t> height_additions;
    int index{0};
    auto to_distribute = height_left;
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Expanding ||
            policy == Size_policy::MinimumExpanding) {
            height_additions.push_back(
                (std::get<0>(tup)->geometry().size_policy().vertical_stretch /
                 static_cast<double>(total_stretch)) *
                to_distribute);
            if ((std::get<2>(tup).get() + height_additions.back()) >
                std::get<0>(tup)->geometry().max_height()) {
                height_left += std::get<0>(tup)->geometry().max_height() -
                               std::get<2>(tup);
                std::get<2>(tup).get() =
                    std::get<0>(tup)->geometry().max_height();
                widgets.erase(std::begin(widgets) + index);
                return distribute_space(widgets, height_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign calculated
    // values
    for (auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Expanding ||
            policy == Size_policy::MinimumExpanding) {
            std::get<2>(tup).get() += height_additions.front();
            height_left -= height_additions.front();
            height_additions.pop_front();
        }
    }

    // SECOND GROUP - duplicate of above dependent on Policies to work with.
    // Preferred and Minimum
    if (height_left == 0) {
        return;
    }
    // Find total stretch
    total_stretch = 0;
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Preferred ||
            policy == Size_policy::Minimum || policy == Size_policy::Ignored) {
            total_stretch +=
                std::get<0>(tup)->geometry().size_policy().vertical_stretch;
        }
    }

    // Calculate new heights of widgets in new group, if any go over max_height
    // then assign max value and recurse without that widget in vector.
    height_additions.clear();
    index = 0;
    to_distribute = height_left;
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Preferred ||
            policy == Size_policy::Minimum || policy == Size_policy::Ignored) {
            height_additions.push_back(
                (std::get<0>(tup)->geometry().size_policy().horizontal_stretch /
                 static_cast<double>(total_stretch)) *
                to_distribute);
            if ((std::get<2>(tup).get() + height_additions.back()) >
                std::get<0>(tup)->geometry().max_height()) {
                height_left -= std::get<0>(tup)->geometry().max_height() -
                               std::get<2>(tup);
                std::get<2>(tup).get() =
                    std::get<0>(tup)->geometry().max_height();
                widgets.erase(std::begin(widgets) + index);
                return distribute_space(widgets, height_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign calculated
    // values
    for (auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Preferred ||
            policy == Size_policy::Minimum || policy == Size_policy::Ignored) {
            std::get<2>(tup).get() += height_additions.front();
            height_left -= height_additions.front();
            height_additions.pop_front();
        }
    }
    if (height_left == 0) {
        return;
    }
    // Rounding error extra
    // First Group
    auto height_check{0};
    do {
        height_check = height_left;
        for (auto& tup : widgets) {
            auto policy =
                std::get<0>(tup)->geometry().size_policy().vertical_policy;
            if ((policy == Size_policy::Expanding ||
                 policy == Size_policy::MinimumExpanding) &&
                height_left > 0) {
                if (std::get<2>(tup).get() + 1 <=
                    std::get<0>(tup)->geometry().max_height()) {
                    std::get<2>(tup).get() += 1;
                    height_left -= 1;
                }
            }
        }
    } while (height_check != height_left);

    // Second Group
    do {
        height_check = height_left;
        for (auto& tup : widgets) {
            auto policy =
                std::get<0>(tup)->geometry().size_policy().vertical_policy;
            if ((policy == Size_policy::Preferred ||
                 policy == Size_policy::Minimum ||
                 policy == Size_policy::Ignored) &&
                height_left > 0) {
                if (std::get<2>(tup).get() + 1 <=
                    std::get<0>(tup)->geometry().max_height()) {
                    std::get<2>(tup).get() += 1;
                    height_left -= 1;
                }
            }
        }
    } while (height_check != height_left);
}

void Vertical_layout::collect_space(
    std::vector<std::tuple<Widget*,
                           std::reference_wrapper<std::size_t>,
                           std::reference_wrapper<std::size_t>>> widgets,
    int height_left) {
    if (height_left == 0) {
        return;
    }
    // Find total stretch of first group
    std::size_t total_stretch{0};
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Ignored) {
            total_stretch +=
                std::get<0>(tup)->geometry().size_policy().vertical_stretch;
        }
    }

    // Find total of inverse of percentages
    double total_inverse{0};
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Ignored) {
            total_inverse +=
                1 /
                (std::get<0>(tup)->geometry().size_policy().vertical_stretch /
                 static_cast<double>(total_stretch));
        }
    }

    // Calculate new heights of widgets in new group, if any go under min_height
    // then assign min value and recurse without that widget in vector.
    std::deque<std::size_t> height_deductions;
    int index{0};
    auto to_collect = height_left;
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Ignored) {
            height_deductions.push_back(
                ((1 /
                  (std::get<0>(tup)->geometry().size_policy().vertical_stretch /
                   static_cast<double>(total_stretch))) /
                 static_cast<double>(total_inverse)) *
                (to_collect * -1));
            if ((std::get<2>(tup).get() - height_deductions.back()) <
                std::get<0>(tup)->geometry().min_height()) {
                height_left -= std::get<2>(tup).get() -
                               std::get<0>(tup)->geometry().min_height();
                std::get<2>(tup).get() =
                    std::get<0>(tup)->geometry().min_height();
                widgets.erase(std::begin(widgets) + index);
                return collect_space(widgets, height_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign calculated
    // values
    for (auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Ignored) {
            if (std::get<2>(tup).get() >= height_deductions.front()) {
                std::get<2>(tup).get() -= height_deductions.front();
            } else {
                std::get<2>(tup).get() = 0;
            }
            height_left += height_deductions.front();
            height_deductions.pop_front();
        }
    }

    // SECOND GROUP - duplicate of above dependent on Policies to work with.
    if (height_left == 0) {
        return;
    }
    // Find total stretch
    total_stretch = 0;
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Expanding) {
            total_stretch +=
                std::get<0>(tup)->geometry().size_policy().vertical_stretch;
        }
    }

    // Find total of inverse of percentages
    total_inverse = 0;
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Expanding) {
            total_inverse +=
                1 /
                (std::get<0>(tup)->geometry().size_policy().vertical_stretch /
                 static_cast<double>(total_stretch));
        }
    }

    // Calculate new heights of widgets in new group, if any go over max_height
    // then assign max value and recurse without that widget in vector.
    height_deductions.clear();
    index = 0;
    to_collect = height_left;
    for (const auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Expanding) {
            height_deductions.push_back(
                ((1 / (std::get<0>(tup)
                           ->geometry()
                           .size_policy()
                           .horizontal_stretch /
                       static_cast<double>(total_stretch))) /
                 static_cast<double>(total_inverse)) *
                (to_collect * -1));
            if ((std::get<2>(tup).get() - height_deductions.back()) <
                std::get<0>(tup)->geometry().min_height()) {
                height_left += std::get<2>(tup).get() -
                               std::get<0>(tup)->geometry().min_height();
                std::get<2>(tup).get() =
                    std::get<0>(tup)->geometry().min_height();
                widgets.erase(std::begin(widgets) + index);
                return collect_space(widgets, height_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign calculated
    // values
    for (auto& tup : widgets) {
        auto policy =
            std::get<0>(tup)->geometry().size_policy().vertical_policy;
        if (policy == Size_policy::Expanding) {
            if (std::get<2>(tup).get() >= height_deductions.front()) {
                std::get<2>(tup).get() -= height_deductions.front();
            } else {
                std::get<2>(tup).get() = 0;
            }
            height_left += height_deductions.front();
            height_deductions.pop_front();
        }
    }
    // Change this to distribute the space, it might not be too small
    if (height_left != 0) {
        too_small_ = true;
        return;
    }
}

void Vertical_layout::position_widgets(
    const std::vector<std::size_t>& heights) {
    std::vector<Widget*> widgets;
    for (Widget* c : this->children()) {
        widgets.push_back(c);
    }
    if (widgets.size() != heights.size()) {
        return;
    }
    std::size_t y_pos = north_border_offset(this->border());
    std::size_t index{0};
    for (auto& widg : widgets) {
        System::post_event<Move_event>(
            widg, this->x() + west_border_offset(this->border()),
            this->y() + y_pos);
        y_pos += heights.at(index++);
    }
}

void Vertical_layout::update_geometry() {
    auto heights = this->size_widgets();
    this->position_widgets(heights);
}

}  // namespace cppurses
