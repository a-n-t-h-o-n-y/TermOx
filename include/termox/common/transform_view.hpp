#ifndef TERMOX_COMMON_TRANFORM_VIEW_HPP
#define TERMOX_COMMON_TRANFORM_VIEW_HPP
#include <cstddef>
#include <type_traits>
#include <utility>

#include <termox/common/transform_iterator.hpp>

namespace ox {

/// Read only Container view that applies transformation to elements at access.
/** Container is assumed to have a vector like interface. Transformation is
 *  applied at iterator dereference and operator[]. */
template <typename Container, typename Map_fn>
class Transform_view {
   public:
    using Reference =
        std::invoke_result_t<Map_fn, typename Container::reference>;
    using Reference_const =
        std::invoke_result_t<Map_fn, typename Container::const_reference>;

   public:
    Transform_view(Container& c, Map_fn map_fn)
        : container_{c}, map_fn_{std::move(map_fn)}
    {}

   public:
    auto size() const -> std::size_t { return container_.size(); }

    auto empty() const -> bool { return container_.empty(); }

    auto operator[](std::size_t i) const -> Reference_const
    {
        return map_fn_(container_[i]);
    }

    auto operator[](std::size_t i) -> Reference
    {
        return map_fn_(container_[i]);
    }

    auto front() const -> Reference_const
    {
        return map_fn_(container_.front());
    }

    auto front() -> Reference { return map_fn_(container_.front()); }

    auto back() const -> Reference_const { return map_fn_(container_.back()); }

    auto back() -> Reference { return map_fn_(container_.back()); }

    auto begin() const
    {
        return Transform_iterator{std::cbegin(container_), map_fn_};
    }

    auto begin() { return Transform_iterator{std::begin(container_), map_fn_}; }

    auto end() const
    {
        return Transform_iterator{std::cend(container_), map_fn_};
    }

    auto end() { return Transform_iterator{std::end(container_), map_fn_}; }

   private:
    Container& container_;
    Map_fn map_fn_;
};

}  // namespace ox
#endif  // TERMOX_COMMON_TRANFORM_VIEW_HPP
