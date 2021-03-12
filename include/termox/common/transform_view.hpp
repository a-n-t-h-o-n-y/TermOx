#ifndef TERMOX_COMMON_TRANFORM_VIEW_HPP
#define TERMOX_COMMON_TRANFORM_VIEW_HPP
#include <cstddef>
#include <type_traits>
#include <utility>

#include <termox/common/transform_iterator.hpp>

namespace ox {

/// Read only Container view that applies transformation to elements at access.
/** Container is assumed to have a vector like interface. Transformation is
 *  applied at iterator dereference and operator[]. This is a non-owning view,
 *  relies on passed in container being alive as long as this view is.\*/
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
    [[nodiscard]] auto size() const -> std::size_t { return container_.size(); }

    [[nodiscard]] auto is_empty() const -> bool { return container_.empty(); }

    [[nodiscard]] auto operator[](std::size_t i) const -> Reference_const
    {
        return map_fn_(container_[i]);
    }

    [[nodiscard]] auto operator[](std::size_t i) -> Reference
    {
        return map_fn_(container_[i]);
    }

    [[nodiscard]] auto front() const -> Reference_const
    {
        return map_fn_(container_.front());
    }

    [[nodiscard]] auto front() -> Reference
    {
        return map_fn_(container_.front());
    }

    [[nodiscard]] auto back() const -> Reference_const
    {
        return map_fn_(container_.back());
    }

    [[nodiscard]] auto back() -> Reference
    {
        return map_fn_(container_.back());
    }

    [[nodiscard]] auto begin() const
    {
        return Transform_iterator{std::cbegin(container_), map_fn_};
    }

    [[nodiscard]] auto begin()
    {
        return Transform_iterator{std::begin(container_), map_fn_};
    }

    [[nodiscard]] auto end() const
    {
        return Transform_iterator{std::cend(container_), map_fn_};
    }

    [[nodiscard]] auto end()
    {
        return Transform_iterator{std::end(container_), map_fn_};
    }

   private:
    Container& container_;
    Map_fn map_fn_;
};

}  // namespace ox
#endif  // TERMOX_COMMON_TRANFORM_VIEW_HPP
