#ifndef TERMOX_COMMON_FILTER_VIEW_HPP
#define TERMOX_COMMON_FILTER_VIEW_HPP
#include <cstddef>
#include <type_traits>
#include <utility>

#include <termox/common/filter_iterator.hpp>

namespace ox {

/// Read only Container view with Filter_iterator interface.
/** The container is held by reference, non-owning. Mostly convinience for range
 *  for loops. Filter_iterator does the heavy lifting. */
template <typename Container, typename Filter_fn>
class Filter_view {
   public:
    Filter_view(Container& c, Filter_fn filter_fn)
        : container_{c}, filter_fn_{std::move(filter_fn)}
    {}

   public:
    [[nodiscard]] auto begin() const
    {
        return Filter_iterator{std::cbegin(container_), std::cend(container_),
                               filter_fn_};
    }

    [[nodiscard]] auto begin()
    {
        return Filter_iterator{std::begin(container_), std::end(container_),
                               filter_fn_};
    }

    [[nodiscard]] auto end() const
    {
        return Filter_iterator{std::cend(container_), std::cend(container_),
                               filter_fn_};
    }

    [[nodiscard]] auto end()
    {
        return Filter_iterator{std::end(container_), std::end(container_),
                               filter_fn_};
    }

   private:
    Container& container_;
    Filter_fn filter_fn_;
};

/// Container view with Filter_iterator interface.
/** The container is copied/moved into the view. Mostly convinience for range
 *  for loops. Filter_iterator does the heavy lifting. */
template <typename Container, typename Filter_fn>
class Owning_filter_view {
   public:
    Owning_filter_view(Container c, Filter_fn filter_fn)
        : container_{std::move(c)}, filter_fn_{std::move(filter_fn)}
    {}

   public:
    [[nodiscard]] auto begin() const
    {
        return Filter_iterator{std::cbegin(container_), std::cend(container_),
                               filter_fn_};
    }

    [[nodiscard]] auto begin()
    {
        return Filter_iterator{std::begin(container_), std::end(container_),
                               filter_fn_};
    }

    [[nodiscard]] auto end() const
    {
        return Filter_iterator{std::cend(container_), std::cend(container_),
                               filter_fn_};
    }

    [[nodiscard]] auto end()
    {
        return Filter_iterator{std::end(container_), std::end(container_),
                               filter_fn_};
    }

   private:
    Container container_;
    Filter_fn filter_fn_;
};

}  // namespace ox
#endif  // TERMOX_COMMON_FILTER_VIEW_HPP
