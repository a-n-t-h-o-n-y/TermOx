#ifndef TERMOX_WIDGET_PAIR_HPP
#define TERMOX_WIDGET_PAIR_HPP
#include <memory>

#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/stack.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <type_traits>

namespace ox {

/// Heterogeneous pair of Widgets within a Layout.
template <typename Layout_t, typename First, typename Second>
struct Pair : Layout_t {
   public:
    struct Parameters {
        typename First::Parameters first;
        typename Second::Parameters second;
    };

   public:
    First& first;
    Second& second;

   public:
    /// Widgets are default constructed.
    Pair()
        : first{this->template make_child<First>()},
          second{this->template make_child<Second>()}
    {}

    explicit Pair(Parameters p)
        : first{this->make_child_maybe_with_parameters<First>(
              std::move(p.first))},
          second{this->make_child_maybe_with_parameters<Second>(
              std::move(p.second))}
    {}

    /// Widgets are constructed with passed in First/Second::Parameters objects.
    Pair(typename First::Parameters a, typename Second::Parameters b)
        : first{this->make_child_maybe_with_parameters<First>(std::move(a))},
          second{this->make_child_maybe_with_parameters<Second>(std::move(b))}
    {}

    /// Existing Widgets are moved into the Pair.
    Pair(std::unique_ptr<First> a, std::unique_ptr<Second> b)
        : Layout_t{std::move(a), std::move(b)},
          first{static_cast<First&>(this->get_children()[0])},
          second{static_cast<Second&>(this->get_children()[1])}
    {}

   public:
    /// Get child by index.
    template <std::size_t Index>
    [[nodiscard]] auto get() -> auto&
    {
        static_assert(Index < 2, "Pair::get() only has two elements.");
        if constexpr (Index == 0)
            return first;
        else if constexpr (Index == 1)
            return second;
    }

    /// Get child by index.
    template <std::size_t Index>
    [[nodiscard]] auto get() const -> auto const&
    {
        static_assert(Index < 2, "Pair::get() only has two elements.");
        if constexpr (Index == 0)
            return first;
        else if constexpr (Index == 1)
            return second;
    }

   private:
    using Layout_t::append_child;
    using Layout_t::delete_all_children;
    using Layout_t::insert_child;
    using Layout_t::make_child;
    using Layout_t::remove_and_delete_child;
    using Layout_t::remove_and_delete_child_at;
    using Layout_t::remove_and_delete_child_if;
    using Layout_t::remove_child;
    using Layout_t::remove_child_at;
    using Layout_t::remove_child_if;
    using Layout_t::swap_children;

   private:
    template <typename Widget_t>
    [[nodiscard]] auto make_child_maybe_with_parameters(
        typename Widget_t::Parameters p) -> Widget_t&
    {
        if constexpr (std::is_constructible_v<Widget_t,
                                              typename Widget_t::Parameters>) {
            return this->template make_child<Widget_t>(std::move(p));
        }
        else
            return this->template make_child<Widget_t>();
    }
};

/// Build a default constructed Pair unique_ptr.
template <typename Layout_t, typename First, typename Second>
[[nodiscard]] auto pair() -> std::unique_ptr<Pair<Layout_t, First, Second>>
{
    return std::make_unique<Pair<Layout_t, First, Second>>();
}

/// Build a Pair unique_ptr from First/Second::Parameter objects.
template <typename Layout_t, typename First, typename Second>
[[nodiscard]] auto pair(typename First::Parameters a,
                        typename Second::Parameters b)
    -> std::unique_ptr<Pair<Layout_t, First, Second>>
{
    return std::make_unique<Pair<Layout_t, First, Second>>(std::move(a),
                                                           std::move(b));
}

/// Build a Pair from existing Widgets.
template <typename Layout_t, typename First, typename Second>
[[nodiscard]] auto pair(std::unique_ptr<First> a, std::unique_ptr<Second> b)
    -> std::unique_ptr<Pair<Layout_t, First, Second>>
{
    return std::make_unique<Pair<Layout_t, First, Second>>(std::move(a),
                                                           std::move(b));
}

/// A Vertical Linear Layout Pair.
template <typename First, typename Second>
using VPair = Pair<layout::Vertical<>, First, Second>;

/// Build a default constructed VPair unique_ptr.
template <typename First, typename Second>
[[nodiscard]] auto vpair() -> std::unique_ptr<VPair<First, Second>>
{
    return std::make_unique<VPair<First, Second>>();
}

/// Build a VPair unique_ptr from First/Second::Parameter objects.
template <typename First, typename Second>
[[nodiscard]] auto vpair(typename First::Parameters a,
                         typename Second::Parameters b)
    -> std::unique_ptr<VPair<First, Second>>
{
    return std::make_unique<VPair<First, Second>>(std::move(a), std::move(b));
}

/// Build a VPair from existing Widgets.
template <typename First, typename Second>
[[nodiscard]] auto vpair(std::unique_ptr<First> a, std::unique_ptr<Second> b)
    -> std::unique_ptr<VPair<First, Second>>
{
    return std::make_unique<VPair<First, Second>>(std::move(a), std::move(b));
}

/// A Horizontal Linear Layout Pair.
template <typename First, typename Second>
using HPair = Pair<layout::Horizontal<>, First, Second>;

/// Build a default constructed HPair unique_ptr.
template <typename First, typename Second>
[[nodiscard]] auto hpair() -> std::unique_ptr<HPair<First, Second>>
{
    return std::make_unique<HPair<First, Second>>();
}

/// Build an HPair unique_ptr from First/Second::Parameter objects.
template <typename First, typename Second>
[[nodiscard]] auto hpair(typename First::Parameters a,
                         typename Second::Parameters b)
    -> std::unique_ptr<HPair<First, Second>>
{
    return std::make_unique<HPair<First, Second>>(std::move(a), std::move(b));
}

/// Build an HPair from existing Widgets.
template <typename First, typename Second>
[[nodiscard]] auto hpair(std::unique_ptr<First> a, std::unique_ptr<Second> b)
    -> std::unique_ptr<HPair<First, Second>>
{
    return std::make_unique<HPair<First, Second>>(std::move(a), std::move(b));
}

/// A Stack Layout Pair.
template <typename First, typename Second>
using SPair = Pair<layout::Stack<>, First, Second>;

/// Build a default constructed SPair unique_ptr.
template <typename First, typename Second>
[[nodiscard]] auto spair() -> std::unique_ptr<SPair<First, Second>>
{
    return std::make_unique<SPair<First, Second>>();
}

/// Build an SPair unique_ptr from First/Second::Parameter objects.
template <typename First, typename Second>
[[nodiscard]] auto spair(typename First::Parameters a,
                         typename Second::Parameters b)
    -> std::unique_ptr<SPair<First, Second>>
{
    return std::make_unique<SPair<First, Second>>(std::move(a), std::move(b));
}

/// Build an SPair from existing Widgets.
template <typename First, typename Second>
[[nodiscard]] auto spair(std::unique_ptr<First> a, std::unique_ptr<Second> b)
    -> std::unique_ptr<SPair<First, Second>>
{
    return std::make_unique<SPair<First, Second>>(std::move(a), std::move(b));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_PAIR_HPP
