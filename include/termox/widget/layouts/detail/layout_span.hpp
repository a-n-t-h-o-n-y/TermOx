#ifndef TERMOX_WIDGET_LAYOUTS_DETAIL_LAYOUT_SPAN_HPP
#define TERMOX_WIDGET_LAYOUTS_DETAIL_LAYOUT_SPAN_HPP
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <numeric>
#include <utility>
#include <vector>

#include <termox/widget/detail/border_offset.hpp>
#include <termox/widget/size_policy.hpp>
#include <termox/widget/widget.hpp>

namespace ox::layout::detail {

struct Dimension {
    Widget* widget;  // This is nullptr when limit is reached
    std::size_t length;
};

/// Container view to iterate over a Widget's children, yielding layout info.
/** Shrinks on each iteration by dropping elements that have reached some limit.
 *  \p Get_policy_t: A functor type <Size_policy const&(Widget const&)>
 *                   Used to retrieve a Size_policy to work with. */
template <typename Get_policy_t>
class Layout_span {
   private:
    using Container_t = std::vector<Dimension>;

    /// \p Get_limit_t Is a functor type <std::size_t(Size_policy const&)>
    template <typename Get_limit_t>
    class Iterator {
       private:
        using Underlying_t = Container_t::iterator;

       public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = Underlying_t::value_type;
        using difference_type   = Underlying_t::difference_type;
        using pointer           = Underlying_t::pointer;
        using reference         = Underlying_t::reference;

       public:
        /// This will increment the position in the container until it is valid.
        Iterator(Underlying_t iter,
                 Underlying_t end,
                 Get_policy_t get_policy,
                 Get_limit_t get_limit)
            : iter_{iter},
              end_{end},
              get_policy_{get_policy},
              get_limit_{get_limit}
        {
            while (iter_ != end && iter_->widget == nullptr)
                ++iter_;
        }

        Iterator()                = delete;
        Iterator(Iterator const&) = delete;
        Iterator(Iterator&&)      = delete;
        auto operator=(Iterator const&) -> Iterator& = delete;
        auto operator=(Iterator&&) -> Iterator& = delete;
        ~Iterator()                             = default;

        auto operator++() -> Iterator&
        {
            if (iter_->length == get_limit_(get_policy_(*iter_->widget)))
                iter_->widget = nullptr;
            do {
                ++iter_;
            } while (iter_ != end_ && iter_->widget == nullptr);
            return *this;
        }

        auto operator*() const -> reference { return *iter_; }

        auto operator->() const -> pointer { return iter_.operator->(); }

        auto operator==(Container_t::iterator other) const -> bool
        {
            return this->iter_ == other;
        }

        auto operator!=(Container_t::iterator other) const -> bool
        {
            return this->iter_ != other;
        }

        [[nodiscard]] auto get_policy() const -> Size_policy const&
        {
            return get_policy_(*iter_->widget);
        }

       private:
        Underlying_t iter_;
        Underlying_t end_;
        Get_policy_t get_policy_;
        Get_limit_t get_limit_;
    };

    // Needed for clang, GCC finds Constructor without this, but class scope
    // deduction guide breaks GCC.
#ifdef __clang__
    template <typename Get_limit_t>
    Iterator(typename Layout_span::Container_t::iterator iter,
             typename Layout_span::Container_t::iterator end,
             Get_policy_t get_policy,
             Get_limit_t get_limit) -> Iterator<decltype(get_limit)>;
#endif

   public:
    /// Construct, only considers children from \p first up to \p last.
    template <typename Iter>
    Layout_span(Iter first,
                Iter last,
                std::size_t primary_length,
                Get_policy_t&& get_policy,
                Policy_direction d)
        : dimensions_{Layout_span::build_dimensions(first,
                                                    last,
                                                    primary_length,
                                                    get_policy,
                                                    d)},
          get_policy_{std::forward<Get_policy_t>(get_policy)}
    {}

    /// Return iterator to the first element, will skip when length == max
    /** Size_policy::max will be used as limit. */
    auto begin_max()
    {
        total_stretch_ = this->calculate_total_stretch();
        return this->begin([](Size_policy const& p) { return p.max(); });
    }

    /// Return iterator to the first element, will skip when length == min
    /** Size_policy::min will be used as limit. */
    auto begin_min()
    {
        total_inverse_stretch_ = this->calculate_total_inverse_stretch();
        return this->begin([](Size_policy const& p) { return p.min(); });
    }

    auto end() -> Container_t::iterator { return dimensions_.end(); }

    auto total_stretch() const -> double { return total_stretch_; }

    auto total_inverse_stretch() const -> double
    {
        return total_inverse_stretch_;
    }

    auto entire_length() const -> std::size_t
    {
        return std::accumulate(dimensions_.begin(), dimensions_.end(), 0uL,
                               [](std::size_t total, Dimension const& d) {
                                   return total + d.length;
                               });
    }

    auto size() const -> std::size_t
    {
        return std::count_if(dimensions_.begin(), dimensions_.end(),
                             [](auto const& d) { return d.widget != nullptr; });
    }

    auto get_results() const -> std::vector<std::size_t>
    {
        auto result = std::vector<std::size_t>{};
        result.reserve(dimensions_.size());
        std::transform(dimensions_.begin(), dimensions_.end(),
                       std::back_inserter(result),
                       [](auto const& d) { return d.length; });
        return result;
    }

   private:
    Container_t dimensions_;
    Get_policy_t get_policy_;
    mutable double total_stretch_         = 0.;
    mutable double total_inverse_stretch_ = 0.;

   private:
    /// Generate a Dimensions container initialized with child Widget pointers.
    template <typename Iter>
    static auto generate_zero_init_dimensions(Iter first, Iter last)
        -> Container_t
    {
        auto result = Container_t{};
        result.reserve(std::distance(first, last));
        std::transform(first, last, std::back_inserter(result),
                       [](auto& child) -> Dimension {
                           return {&child, 0uL};
                       });
        return result;
    }

    template <typename Iter_t>  // Dimension iterator
    static void handle_edge(Iter_t first,
                            Iter_t last,
                            std::size_t leftover,
                            bool can_ignore_min)
    {
        // First iter is edge, can assume it is not last.
        if (can_ignore_min)
            first->length = leftover;
        else
            first->length = 0uL;
        ++first;
        while (first != last) {
            first->length = 0uL;
            ++first;
        }
    }

    static void invalidate_each(Container_t& dimensions)
    {
        for (auto& d : dimensions)
            d.widget = nullptr;
    }

    /// Set each Dimension to the cooresponding Widget's min.
    /** If the running total of min goes over \p length, then give that edge
     *  widget the last of the space(if Size_policy::ignore_min is true). Next,
     *  set all following widgets to length zero. If the total of min does not
     *  exceed \p length, then return false. */
    static auto set_each_to_min(Container_t& dimensions,
                                std::size_t length,
                                Get_policy_t get_policy,
                                Policy_direction d) -> bool
    {
        // Can assume all Dimension::widget pointers are valid.
        auto min_sum   = 0uL;
        auto const end = std::end(dimensions);
        for (auto iter = std::begin(dimensions); iter != end; ++iter) {
            auto const& policy = get_policy(*(iter->widget));
            auto const min     = [&] {
                if (policy.is_passive())
                    return sum_child_mins(*iter->widget, get_policy, d);
                return policy.min();
            }();
            min_sum += min;
            if (min_sum > length) {  // Stop Condition
                auto const leftover = length - (min_sum - min);
                handle_edge(iter, end, leftover, policy.can_ignore_min());
                invalidate_each(dimensions);
                return true;
            }
            iter->length = min;
        }
        return false;
    }

    /// Set each Dimension to the cooresponding Widget's hint.
    static auto set_each_to_hint(Container_t& dimensions,
                                 Get_policy_t get_policy)
    {
        // Can assume all Dimension::widget pointers are valid.
        std::for_each(std::begin(dimensions), std::end(dimensions),
                      [get_policy](auto& dimension) {
                          auto const& policy = get_policy(*dimension.widget);
                          if (policy.is_passive())
                              return;
                          dimension.length = policy.hint();
                      });
    }

    template <typename Iter>
    static auto build_dimensions(Iter first,
                                 Iter last,
                                 std::size_t primary_length,
                                 Get_policy_t get_policy,
                                 Policy_direction d) -> Container_t
    {
        auto dimensions = generate_zero_init_dimensions(first, last);
        if (!set_each_to_min(dimensions, primary_length, get_policy, d))
            set_each_to_hint(dimensions, get_policy);
        return dimensions;
    }

    /// \p Get_limit_t Is a functor type <std::size_t(Size_policy const&)>
    /** Calculates and stores total stretch when called. */
    template <typename Get_limit_t>
    auto begin(Get_limit_t get_limit)
    {
        auto const begin = dimensions_.begin();
        auto const end   = dimensions_.end();
        auto temp        = Iterator{begin, end, get_policy_, get_limit};
        while (temp != end)
            ++temp;  // This call invalidates elements that are at limit.
        return Iterator{begin, end, get_policy_, get_limit};
    }

    auto calculate_total_stretch() const -> double
    {
        auto sum       = 0.;
        auto const end = dimensions_.end();
        for (auto iter = dimensions_.begin(); iter != end; ++iter) {
            if (iter->widget != nullptr)
                sum += get_policy_(*iter->widget).stretch();
        }
        return sum;
    }

    auto calculate_total_inverse_stretch() const -> double
    {
        auto sum       = 0.;
        auto const end = dimensions_.end();
        for (auto iter = dimensions_.begin(); iter != end; ++iter) {
            if (iter->widget != nullptr)
                sum += (1. / get_policy_(*iter->widget).stretch());
        }
        return sum;
    }

    auto find_valid_begin() const
    {
        auto const end = dimensions_.end();
        for (auto iter = dimensions_.begin(); iter != end; ++iter) {
            if (iter->widget != nullptr)
                return iter;
        }
        return end;
    }

    /// Sum policy.min() of each child, if child is passive this is recursive.
    static auto sum_child_mins(Widget const& w,
                               Get_policy_t get_policy,
                               Policy_direction d) -> std::size_t
    {
        // TODO Calling get_policy on child objects will break when child is not
        // the same layout type as the parent.
        auto sum            = 0uL;
        auto const children = w.get_children();
        for (auto i = w.get_child_offset(); i < w.child_count(); ++i) {
            auto const& policy = get_policy(children[i]);
            if (policy.is_passive())
                sum += sum_child_mins(children[i], get_policy, d);
            else
                sum += policy.min();
        }
        if (get_policy(w).is_passive()) {
            switch (d) {
                using namespace ox::detail;
                case Policy_direction::Vertical:
                    sum += Border_offset::north_enabled(w) ? 1 : 0;
                    sum += Border_offset::south_enabled(w) ? 1 : 0;
                    break;
                case Policy_direction::Horizontal:
                    sum += Border_offset::east_enabled(w) ? 1 : 0;
                    sum += Border_offset::west_enabled(w) ? 1 : 0;
                    break;
            }
        }
        return sum;
    }
};

}  // namespace ox::layout::detail
#endif  // TERMOX_WIDGET_LAYOUTS_DETAIL_LAYOUT_SPAN_HPP
