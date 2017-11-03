#include <cppurses/system/events/move_event.hpp>
#include <cppurses/system/events/resize_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/layouts/horizontal_layout.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>

#include <cstddef>
#include <deque>
#include <functional>
#include <iterator>
#include <vector>

namespace cppurses {

std::vector<std::size_t> Horizontal_layout::size_widgets() {
    std::vector<Dimensions> widgets;
    std::size_t total_stretch{0};
    for (Widget* c : this->children()) {
        if (c->visible()) {
            widgets.emplace_back(Dimensions{c, 0, 0});
            total_stretch += c->width_policy.stretch();
        }
    }

    int width_available = this->width();

    // HORIZONTAL
    // Set Fixed, Minimum and MinimumExpanding to width_hint
    for (Dimensions& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Fixed || policy == Size_policy::Minimum ||
            policy == Size_policy::MinimumExpanding) {
            d.width = d.widget->width_policy.hint();
            width_available -= d.width;
        }
    }
    if (width_available < 0) {
        too_small_ = true;
        return std::vector<std::size_t>();
    }

    // Set Size_policy::Ignored widgets to their stretch factor width value
    for (Dimensions& d : widgets) {
        if (d.widget->width_policy.type() == Size_policy::Ignored) {
            const float percent = d.widget->width_policy.stretch() /
                                  static_cast<float>(total_stretch);
            std::size_t width = percent * this->width();
            if (width < d.widget->width_policy.min()) {
                width = d.widget->width_policy.min();
            } else if (width > d.widget->width_policy.max()) {
                width = d.widget->width_policy.max();
            }
            d.width = width;
            width_available -= width;
        }
    }

    // Set Maximum, Preferred and Expanding to width_hint
    for (Dimensions& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Expanding) {
            d.width = d.widget->width_policy.hint();
            width_available -= d.width;
        }
    }

    // create vector of size references for below if statements
    std::vector<Dimensions_reference> widgets_w_refs;
    for (Dimensions& d : widgets) {
        widgets_w_refs.emplace_back(
            Dimensions_reference{d.widget, &d.width, &d.height});
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
    for (Dimensions& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Fixed) {
            d.height = d.widget->height_policy.hint();
            if (d.height > this->height()) {
                too_small_ = true;
                return std::vector<std::size_t>();
            }
        } else if (policy == Size_policy::Ignored ||
                   policy == Size_policy::Preferred ||
                   policy == Size_policy::Expanding) {
            d.height = this->height();
            if (d.height > d.widget->height_policy.max()) {
                d.height = d.widget->height_policy.max();
            } else if (d.height < d.widget->height_policy.min()) {
                too_small_ = true;
                return std::vector<std::size_t>();
            }
        } else if (policy == Size_policy::Maximum) {
            d.height = this->height();
            if (d.height > d.widget->height_policy.hint()) {
                d.height = d.widget->height_policy.hint();
            } else if (d.height < d.widget->height_policy.min()) {
                too_small_ = true;
                return std::vector<std::size_t>();
            }
        } else if (policy == Size_policy::Minimum ||
                   policy == Size_policy::MinimumExpanding) {
            d.height = this->height();
            if (d.height < d.widget->height_policy.hint()) {
                d.height = d.widget->height_policy.hint();
            }
            if (d.height > d.widget->height_policy.max() ||
                d.height > this->height()) {
                too_small_ = true;
                return std::vector<std::size_t>();
            }
        }
    }

    // Post all Resize_events
    for (Dimensions& d : widgets) {
        System::post_event<Resize_event>(d.widget, Area{d.width, d.height});
    }
    std::vector<std::size_t> widths;
    widths.reserve(widgets.size());
    for (const Dimensions& d : widgets) {
        widths.push_back(d.width);
    }
    return widths;
}

void Horizontal_layout::distribute_space(
    std::vector<Dimensions_reference> widgets,
    int width_left) {
    // Find total stretch of first group
    std::size_t total_stretch{0};
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Expanding ||
            policy == Size_policy::MinimumExpanding) {
            total_stretch += d.widget->width_policy.stretch();
        }
    }

    // Calculate new widths of widgets in new group, if any go over max_width
    // then assign max value and recurse without that widget in vector.
    std::deque<std::size_t> width_additions;
    int index{0};
    auto to_distribute = width_left;
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Expanding ||
            policy == Size_policy::MinimumExpanding) {
            width_additions.push_back((d.widget->width_policy.stretch() /
                                       static_cast<double>(total_stretch)) *
                                      to_distribute);
            if ((*d.width + width_additions.back()) >
                d.widget->width_policy.max()) {
                width_left -= d.widget->width_policy.max() - *d.width;
                *d.width = d.widget->width_policy.max();
                widgets.erase(std::begin(widgets) + index);
                return distribute_space(widgets, width_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign values
    for (Dimensions_reference& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Expanding ||
            policy == Size_policy::MinimumExpanding) {
            *d.width += width_additions.front();
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
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Preferred ||
            policy == Size_policy::Minimum || policy == Size_policy::Ignored) {
            total_stretch += d.widget->width_policy.stretch();
        }
    }

    // Calculate new widths of widgets in new group, if any go over max_width
    // then assign max value and recurse without that widget in vector.
    width_additions.clear();
    index = 0;
    to_distribute = width_left;
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Preferred ||
            policy == Size_policy::Minimum || policy == Size_policy::Ignored) {
            width_additions.push_back((d.widget->width_policy.stretch() /
                                       static_cast<double>(total_stretch)) *
                                      to_distribute);
            if ((*d.width + width_additions.back()) >
                d.widget->width_policy.max()) {
                width_left -= d.widget->width_policy.max() - *d.width;
                *d.width = d.widget->width_policy.max();
                widgets.erase(std::begin(widgets) + index);
                return distribute_space(widgets, width_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign values
    for (Dimensions_reference& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Preferred ||
            policy == Size_policy::Minimum || policy == Size_policy::Ignored) {
            *d.width += width_additions.front();
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
        for (Dimensions_reference& d : widgets) {
            auto policy = d.widget->width_policy.type();
            if ((policy == Size_policy::Expanding ||
                 policy == Size_policy::MinimumExpanding) &&
                width_left > 0) {
                if (*d.width + 1 <= d.widget->width_policy.max()) {
                    *d.width += 1;
                    width_left -= 1;
                }
            }
        }
    } while (width_check != width_left);

    // Second Group
    do {
        width_check = width_left;
        for (Dimensions_reference& d : widgets) {
            auto policy = d.widget->width_policy.type();
            if ((policy == Size_policy::Preferred ||
                 policy == Size_policy::Minimum ||
                 policy == Size_policy::Ignored) &&
                width_left > 0) {
                if (*d.width + 1 <= d.widget->width_policy.max()) {
                    *d.width += 1;
                    width_left -= 1;
                }
            }
        }
    } while (width_check != width_left);
}

void Horizontal_layout::collect_space(std::vector<Dimensions_reference> widgets,
                                      int width_left) {
    if (width_left == 0) {
        return;
    }
    // Find total stretch of first group
    std::size_t total_stretch{0};
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Ignored) {
            total_stretch += d.widget->width_policy.stretch();
        }
    }

    // Find total of inverse of percentages
    double total_inverse{0};
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Ignored) {
            total_inverse += 1 / (d.widget->width_policy.stretch() /
                                  static_cast<double>(total_stretch));
        }
    }

    // Calculate new widths of widgets in new group, if any go under min_width
    // then assign min value and recurse without that widget in vector.
    std::deque<std::size_t> width_deductions;
    int index{0};
    auto to_collect = width_left;
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Ignored) {
            width_deductions.push_back(
                ((1 / (d.widget->width_policy.stretch() /
                       static_cast<double>(total_stretch))) /
                 static_cast<double>(total_inverse)) *
                (to_collect * -1));
            if ((*d.width - width_deductions.back()) <
                d.widget->width_policy.min()) {
                width_left += *d.width - d.widget->width_policy.min();
                *d.width = d.widget->width_policy.min();
                widgets.erase(std::begin(widgets) + index);
                return collect_space(widgets, width_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign calculated
    // values
    for (Dimensions_reference& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Maximum ||
            policy == Size_policy::Preferred ||
            policy == Size_policy::Ignored) {
            if (*d.width >= width_deductions.front()) {
                *d.width -= width_deductions.front();
            } else {
                *d.width = 0;
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
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Expanding) {
            total_stretch += d.widget->width_policy.stretch();
        }
    }

    // Find total of inverse of percentages
    total_inverse = 0;
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Expanding) {
            total_inverse += 1 / (d.widget->width_policy.stretch() /
                                  static_cast<double>(total_stretch));
        }
    }

    // Calculate new widths of widgets in new group, if any go over max_width
    // then assign max value and recurse without that widget in vector.
    width_deductions.clear();
    index = 0;
    to_collect = width_left;
    for (const Dimensions_reference& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Expanding) {
            width_deductions.push_back(
                ((1 / (d.widget->width_policy.stretch() /
                       static_cast<double>(total_stretch))) /
                 static_cast<double>(total_inverse)) *
                (to_collect * -1));
            if ((*d.width - width_deductions.back()) <
                d.widget->width_policy.min()) {
                width_left += *d.width - d.widget->width_policy.min();
                *d.width = d.widget->width_policy.min();
                widgets.erase(std::begin(widgets) + index);
                return collect_space(widgets, width_left);
            }
        }
        ++index;
    }

    // If it has gotten this far, no widgets were over space, assign calculated
    // values
    for (Dimensions_reference& d : widgets) {
        auto policy = d.widget->width_policy.type();
        if (policy == Size_policy::Expanding) {
            if (*d.width >= width_deductions.front()) {
                *d.width -= width_deductions.front();
            } else {
                *d.width = 0;
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
    std::vector<Widget*> widgets{this->children()};
    if (widgets.size() != widths.size()) {
        return;
    }
    std::size_t index{0};
    std::size_t y_offset{north_border_offset(*this)};
    std::size_t x_offset{west_border_offset(*this)};
    for (Widget* w : widgets) {
        std::size_t x_pos{this->x() - west_border_offset(*this) + x_offset};
        std::size_t y_pos{this->y() - north_border_offset(*this) + y_offset};
        System::post_event<Move_event>(w, Point{x_pos, y_pos});
        x_offset += widths.at(index++);
    }
}

void Horizontal_layout::update_geometry() {
    auto widths = this->size_widgets();
    this->position_widgets(widths);
}

}  // namespace cppurses
