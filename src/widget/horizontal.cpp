#include <cppurses/widget/layouts/horizontal.hpp>

#include <cstddef>
#include <deque>
#include <iterator>
#include <vector>

#include <cppurses/system/events/move_event.hpp>
#include <cppurses/system/events/resize_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace layout {

std::vector<Layout::Dimensions> Horizontal::calculate_widget_sizes() {
    std::vector<Dimensions> widgets;
    std::size_t total_stretch{0};
    for (const std::unique_ptr<Widget>& c : this->children.get()) {
        if (c->enabled()) {
            widgets.emplace_back(Dimensions{c.get(), 0, this->height()});
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
    // if (width_available < 0) {
    //     return widgets;
    // }

    // Set Size_policy::Ignored widgets to their stretch factor width value.
    for (Dimensions& d : widgets) {
        if (d.widget->width_policy.type() == Size_policy::Ignored) {
            const float percent = d.widget->width_policy.stretch() /
                                  static_cast<float>(total_stretch);
            std::size_t width = percent * this->width();
            if (width < d.widget->width_policy.min_size()) {
                width = d.widget->width_policy.min_size();
            } else if (width > d.widget->width_policy.max_size()) {
                width = d.widget->width_policy.max_size();
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

    /// DISTRIBUTE SPACE ------------------------------------------------------

    // create vector of size references for below if statements
    std::vector<Dimensions_reference> widgets_w_refs;
    widgets_w_refs.reserve(widgets.size());
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

    /// DISTRIBUTE SPACE ------------------------------------------------------

    // VERTICAL - repeat the above, but with vertical properties
    for (Dimensions& d : widgets) {
        auto policy = d.widget->height_policy.type();
        if (policy == Size_policy::Fixed) {
            d.height = d.widget->height_policy.hint();
        } else if (policy == Size_policy::Ignored ||
                   policy == Size_policy::Preferred ||
                   policy == Size_policy::Expanding) {
            if (d.height > d.widget->height_policy.max_size()) {
                d.height = d.widget->height_policy.max_size();
            } else if (d.height < d.widget->height_policy.min_size()) {
                d.height = d.widget->height_policy.min_size();
            }
        } else if (policy == Size_policy::Maximum) {
            if (d.height > d.widget->height_policy.hint()) {
                d.height = d.widget->height_policy.hint();
            }
        } else if (policy == Size_policy::Minimum ||
                   policy == Size_policy::MinimumExpanding) {
            if (d.height > d.widget->height_policy.max_size()) {
                d.height = d.widget->height_policy.max_size();
            } else if (d.height < d.widget->height_policy.hint()) {
                d.height = d.widget->height_policy.hint();
            }
        }
    }
    return widgets;
}

// void Horizontal::distribute_space(std::vector<Dimensions>& widgets,
//                                          int width_left) {}

void Horizontal::distribute_space(std::vector<Dimensions_reference> widgets,
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
                d.widget->width_policy.max_size()) {
                width_left -= d.widget->width_policy.max_size() - *d.width;
                *d.width = d.widget->width_policy.max_size();
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
                d.widget->width_policy.max_size()) {
                width_left -= d.widget->width_policy.max_size() - *d.width;
                *d.width = d.widget->width_policy.max_size();
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
                if (*d.width + 1 <= d.widget->width_policy.max_size()) {
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
                if (*d.width + 1 <= d.widget->width_policy.max_size()) {
                    *d.width += 1;
                    width_left -= 1;
                }
            }
        }
    } while (width_check != width_left);
}

// void Horizontal::collect_space(std::vector<Dimensions>& widgets,
//                                       int width_left) {
//     // Maximum, Preferred, Ignored
//     // find maximum space that you can take back.
//     int available_retake_width{0};
//     for (Dimensions& d : widgets) {
//         Size_policy::Type policy = d.widget->width_policy.type();
//         if (policy == Size_policy::Maximum ||
//             policy == Size_policy::Preferred ||
//             policy == Size_policy::Ignored) {
//             available_retake_width += d.width - d.widget->width_policy.min();
//         }
//     }
//     if (available_retake_width + width_left == 0) {
//         // set everything to its min() in this group and return
//         for (Dimensions& d : widgets) {
//             Size_policy::Type policy = d.widget->width_policy.type();
//             if (policy == Size_policy::Maximum ||
//                 policy == Size_policy::Preferred ||
//                 policy == Size_policy::Ignored) {
//                 d.width = d.widget->width_policy.min();
//             }
//         }
//         return;
//     }
//     if (available_retake_width + width_left < 0) {
//         // set everything to its min() and continue to expanding group
//         for (Dimensions& d : widgets) {
//             Size_policy::Type policy = d.widget->width_policy.type();
//             if (policy == Size_policy::Maximum ||
//                 policy == Size_policy::Preferred ||
//                 policy == Size_policy::Ignored) {
//                 d.width = d.widget->width_policy.min();
//             }
//         }
//     }
//     // just take away until width_left == 0
//     if (available_retake_width + width_left > 0) {
//         // Ignored
//         for (Dimensions& d : widgets) {
//             Size_policy::Type policy = d.widget->width_policy.type();
//             if (policy == Size_policy::Ignored) {
//                 int potential_retake = d.width -
//                 d.widget->width_policy.min();
//                 // give it all
//                 if (potential_retake + width_left < 0) {
//                     d.width = d.widget->width_policy.min();
//                 }
//                 // only give enough and return
//                 if (potential_retake + width_left >= 0) {
//                     d.width += width_left;
//                     return;
//                 }
//             }
//         }
//         // Maximum
//         for (Dimensions& d : widgets) {
//             Size_policy::Type policy = d.widget->width_policy.type();
//             if (policy == Size_policy::Maximum) {
//                 int potential_retake = d.width -
//                 d.widget->width_policy.min();
//                 // give it all
//                 if (potential_retake + width_left < 0) {
//                     d.width = d.widget->width_policy.min();
//                 }
//                 // only give enough and return
//                 if (potential_retake + width_left >= 0) {
//                     d.width += width_left;
//                     return;
//                 }
//             }
//         }
//         // Preferred
//         for (Dimensions& d : widgets) {
//             Size_policy::Type policy = d.widget->width_policy.type();
//             if (policy == Size_policy::Preferred) {
//                 int potential_retake = d.width -
//                 d.widget->width_policy.min();
//                 // give it all
//                 if (potential_retake + width_left < 0) {
//                     d.width = d.widget->width_policy.min();
//                 }
//                 // only give enough and return
//                 if (potential_retake + width_left >= 0) {
//                     d.width += width_left;
//                     return;
//                 }
//             }
//         }
//     }

//     // Expanding
//     available_retake_width = 0;
//     for (Dimensions& d : widgets) {
//         Size_policy::Type policy = d.widget->width_policy.type();
//         if (policy == Size_policy::Expanding) {
//             available_retake_width += d.width - d.widget->width_policy.min();
//         }
//     }
//     if (available_retake_width + width_left <= 0) {
//         // set everything to its min() in this group and return
//         for (Dimensions& d : widgets) {
//             Size_policy::Type policy = d.widget->width_policy.type();
//             if (policy == Size_policy::Expanding) {
//                 d.width = d.widget->width_policy.min();
//             }
//         }
//         return;
//     }
//     if (available_retake_width + width_left > 0) {
//         for (Dimensions& d : widgets) {
//             Size_policy::Type policy = d.widget->width_policy.type();
//             if (policy == Size_policy::Expanding) {
//                 int potential_retake = d.width -
//                 d.widget->width_policy.min();
//                 // give it all
//                 if (potential_retake + width_left < 0) {
//                     d.width = d.widget->width_policy.min();
//                 }
//                 // only give enough and return
//                 if (potential_retake + width_left >= 0) {
//                     d.width += width_left;
//                     return;
//                 }
//             }
//         }
//     }
// }

void Horizontal::collect_space(std::vector<Dimensions_reference> widgets,
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
                d.widget->width_policy.min_size()) {
                width_left += *d.width - d.widget->width_policy.min_size();
                *d.width = d.widget->width_policy.min_size();
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
                d.widget->width_policy.min_size()) {
                width_left += *d.width - d.widget->width_policy.min_size();
                *d.width = d.widget->width_policy.min_size();
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
        return;
    }
}

void Horizontal::move_and_resize_children(
    const std::vector<Dimensions>& dimensions) {
    const std::size_t parent_x{this->inner_x()};
    const std::size_t parent_y{this->inner_y()};
    const std::size_t parent_width{this->width()};
    const std::size_t parent_height{this->height()};
    std::size_t x_pos{parent_x};
    for (const Dimensions& d : dimensions) {
        if ((x_pos + d.width) > (parent_x + parent_width) ||
            (parent_y + d.height) > (parent_y + parent_height) ||
            d.height == 0 || d.width == 0) {
            d.widget->disable(true, false);  // don't send child_polished_events
        } else {
            System::post_event<Move_event>(*(d.widget), Point{x_pos, parent_y});
            System::post_event<Resize_event>(*(d.widget),
                                             Area{d.width, d.height});
            x_pos += d.width;
        }
    }
}

void Horizontal::update_geometry() {
    this->enable(true, false);
    std::vector<Dimensions> widths{this->calculate_widget_sizes()};
    this->move_and_resize_children(widths);
}

}  // namespace layout
}  // namespace cppurses
