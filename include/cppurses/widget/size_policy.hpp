#ifndef WIDGET_SIZE_POLICY_HPP
#define WIDGET_SIZE_POLICY_HPP
#include <cstddef>
#include <limits>

namespace cppurses {
class Widget;

class Size_policy {
   public:
    enum Type {
        Fixed,
        Minimum,
        Maximum,
        Preferred,
        Expanding,
        MinimumExpanding,
        Ignored
    };

    void type(Size_policy::Type type);
    Size_policy::Type type() const;

    void stretch(std::size_t value);
    std::size_t stretch() const;

    void hint(std::size_t value);
    std::size_t hint() const;

    void min(std::size_t value);
    std::size_t min() const;

    void max(std::size_t value);
    std::size_t max() const;

    explicit Size_policy(Widget* owner);

   private:
    Size_policy::Type type_{Type::Ignored};
    std::size_t stretch_{1};
    std::size_t hint_{0};
    std::size_t min_{0};
    std::size_t max_{std::numeric_limits<std::size_t>::max()};
    Widget* owner_;

    void notify_parent() const;
};

}  // namespace cppurses
#endif  // WIDGET_SIZE_POLICY_HPP
