#ifndef CPPURSES_WIDGET_WIDGETS_DETAIL_SLIDER_LOGIC_HPP
#define CPPURSES_WIDGET_WIDGETS_DETAIL_SLIDER_LOGIC_HPP
#include <cstddef>

namespace cppurses {
namespace detail {

/// Holds onto the current value and value limit of a slider like widget.
class Slider_logic {
   public:
    Slider_logic(std::size_t limit) : limit_{limit == 0 ? 1 : limit} {}

    /// Return the current value.
    std::size_t value() const { return current_value_; }

    /// Return the ratio of value / limit
    double ratio() const {
        return static_cast<double>(current_value_) / limit_;
    };

    /// Return one past the largest possible value.
    std::size_t limit() const { return limit_; }

    void set_value(std::size_t value) {
        if (value >= limit_) {
            value = limit_ - 1;
        }
        current_value_ = value;
    }

    void increment(std::size_t amount = 1) {
        this->set_value(this->value() + amount);
    }

    void decrement(std::size_t amount = 1) {
        if (amount > this->value()) {
            this->set_value(0);
        } else {
            this->set_value(this->value() - amount);
        }
    }

   private:
    std::size_t limit_;
    std::size_t current_value_{0};
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_DETAIL_SLIDER_LOGIC_HPP
