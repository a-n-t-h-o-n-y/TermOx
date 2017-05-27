#ifndef PAINTER_MODULE_GEOMETRY_HPP
#define PAINTER_MODULE_GEOMETRY_HPP

#include "widget_module/size_policy.hpp"
#include <cstddef>
#include <limits>

namespace twf {
class Widget;

class Geometry {
   public:
    explicit Geometry(Widget* widget = nullptr) : widget_{widget} {}

    void set_widget(Widget* widget) { widget_ = widget; }

    // Set
    void set_width(std::size_t width) { width_ = width; }
    void set_height(std::size_t height) { height_ = height; }

    void set_width_hint(std::size_t hint);
    void set_height_hint(std::size_t hint);
    void set_max_width(std::size_t max_width);
    void set_max_height(std::size_t max_height);
    void set_min_width(std::size_t min_width);
    void set_min_height(std::size_t min_height);
    void set_size_policy(const Size_policy& policy);

    // Get
    std::size_t width() const { return width_; }
    std::size_t height() const { return height_; }

    std::size_t width_hint() const { return width_hint_; }
    std::size_t height_hint() const { return height_hint_; }
    std::size_t max_width() const { return max_width_; }
    std::size_t max_height() const { return max_height_; }
    std::size_t min_width() const { return min_width_; }
    std::size_t min_height() const { return min_height_; }

    Size_policy& size_policy() { return size_policy_; }
    const Size_policy& size_policy() const { return size_policy_; }

   private:
    Widget* widget_;

    // Parameters for Layouts to use
    std::size_t width_hint_ = 0;
    std::size_t min_width_ = 0;
    std::size_t max_width_ = std::numeric_limits<std::size_t>::max();

    std::size_t height_hint_ = 0;
    std::size_t min_height_ = 0;
    std::size_t max_height_ = std::numeric_limits<std::size_t>::max();

    Size_policy size_policy_;

    // Actual Size
    std::size_t width_ = width_hint_;
    std::size_t height_ = height_hint_;

    void notify_parent() const;
};

}  // namespace twf
#endif  // PAINTER_MODULE_GEOMETRY_HPP
