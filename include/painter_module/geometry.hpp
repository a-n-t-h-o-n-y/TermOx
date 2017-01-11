#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include "../system_module/system.hpp"
#include "../widget_module/size_policy.hpp"

#include <cstddef>
#include <limits>

namespace twf {
class Widget;

class Geometry {
   public:
    explicit Geometry(Widget* widget = nullptr,
                      std::size_t width_hint = System::max_width(),
                      std::size_t height_hint = System::max_height(),
                      Size_policy policy = Size_policy{})
        : widget_{widget},
          width_hint_{width_hint},
          height_hint_{height_hint},
          size_policy_{policy} {}

    void set_widget(Widget* widget) { widget_ = widget; }

    // Set
    void set_width(std::size_t width) { width_ = width; }
    void set_height(std::size_t height) { height_ = height; }
    void set_width_hint(std::size_t width_hint);
    void set_height_hint(std::size_t height_hint);
    void set_max_width(std::size_t max_width);
    void set_max_height(std::size_t max_height);
    void set_min_width(std::size_t min_width);
    void set_min_width_hint(std::size_t min_width_hint);
    void set_min_height(std::size_t min_height);
    void set_min_height_hint(std::size_t min_height_hint);
    void set_size_policy(const Size_policy& policy);

    void set_fixed_width(std::size_t width) {
        this->set_min_width(width);
        this->set_max_width(width);
    }
    void set_fixed_height(std::size_t height) {
        this->set_min_height(height);
        this->set_max_height(height);
    }

    // Get
    std::size_t width() const { return width_; }
    std::size_t height() const { return height_; }

    std::size_t width_hint() const { return width_hint_; }
    std::size_t height_hint() const { return height_hint_; }
    std::size_t max_width() const { return max_width_; }
    std::size_t max_height() const { return max_height_; }
    std::size_t min_width() const { return min_width_; }
    std::size_t min_width_hint() const { return min_width_hint_; }
    std::size_t min_height() const { return min_height_; }
    std::size_t min_height_hint() const { return min_height_hint_; }

    Size_policy size_policy() const { return size_policy_; }

   private:
    Widget* widget_;

    // Parameters for Layouts to use
    std::size_t width_hint_;
    std::size_t height_hint_;
    std::size_t max_width_ = std::numeric_limits<std::size_t>::max();
    std::size_t max_height_ = std::numeric_limits<std::size_t>::max();
    std::size_t min_width_ = 0;
    std::size_t min_width_hint_ = 0;
    std::size_t min_height_ = 0;
    std::size_t min_height_hint_ = 0;

    Size_policy size_policy_;

    // Actual Size
    std::size_t width_ = width_hint_;
    std::size_t height_ = height_hint_;
};

}  // namespace twf
#endif  // GEOMETRY_HPP
