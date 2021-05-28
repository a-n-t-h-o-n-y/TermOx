#ifndef TERMOX_WIDGET_LAYOUTS_PASSIVE_HPP
#define TERMOX_WIDGET_LAYOUTS_PASSIVE_HPP
#include <array>
#include <memory>
#include <type_traits>
#include <utility>

#include <termox/widget/bordered.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/size_policy.hpp>
#include <termox/widget/widget.hpp>

namespace ox::detail {

template <typename W>
inline auto constexpr is_horizontal_or_vertical_v =
    ::ox::layout::is_horizontal_v<W> || ::ox::layout::is_vertical_v<W>;

template <typename W>
struct Is_bordered : std::false_type {};

template <typename W>
struct Is_bordered<::ox::Bordered<W>> : std::true_type {};

template <typename W>
inline auto constexpr is_bordered_v = Is_bordered<W>::value;

}  // namespace ox::detail

namespace ox {
template <typename Layout_t, typename SFINAE = void>
class Passive;

/// Make any Linear_layout type passive.
/** A Passive Layout will always have the size policy that is the sum of its
 *  children's size hints. */
template <typename Layout_t>
class Passive<Layout_t,
              std::enable_if_t<detail::is_horizontal_or_vertical_v<Layout_t> &&
                                   !detail::is_bordered_v<Layout_t>,
                               void>> : public Layout_t {
   private:
    using Base_t = Layout_t;

    static auto constexpr is_vertical = layout::is_vertical_v<Base_t>;

   public:
    using Parameters = typename Base_t::Parameters;

   public:
    template <typename... Args>
    Passive(Args&&... args) : Base_t{std::forward<Args>(args)...}
    {
        this->set_length(
            sum_hints(this->get_children(), this->get_child_offset()));
    }

    Passive(Parameters p) : Base_t{std::move(p)}
    {
        this->set_length(
            sum_hints(this->get_children(), this->get_child_offset()));
    }

   protected:
    auto child_added_event(ox::Widget& child) -> bool override
    {
        this->set_length(
            sum_hints(this->get_children(), this->get_child_offset()));
        return Base_t::child_added_event(child);
    }

    auto child_removed_event(ox::Widget& child) -> bool override
    {
        this->set_length(
            sum_hints(this->get_children(), this->get_child_offset()));
        return Base_t::child_removed_event(child);
    }

    auto child_polished_event(ox::Widget& child) -> bool override
    {
        this->set_length(
            sum_hints(this->get_children(), this->get_child_offset()));
        return Base_t::child_polished_event(child);
    }

   private:
    /// Return the sum of all hints in the inherited from Layout_t
    /// direction.
    static auto sum_hints(decltype(std::declval<Layout_t>().get_children())
                              const& children,
                          std::size_t begin_offset) -> int
    {
        auto get_hint = [](Widget const& child) {
            if constexpr (is_vertical)
                return child.height_policy.hint();
            else
                return child.width_policy.hint();
        };

        auto sum = 0;
        for (auto i = begin_offset; i < children.size(); ++i)
            sum += get_hint(children[i]);
        return sum;
    }

    void set_length(int length)
    {
        if constexpr (is_vertical)
            *this | pipe::fixed_height(length);
        else
            *this | pipe::fixed_width(length);
    }
};

template <typename Wrapped_t>
class Passive<
    Bordered<Wrapped_t>,
    std::enable_if_t<detail::is_horizontal_or_vertical_v<Wrapped_t>, void>>
    : public Bordered<Wrapped_t> {
   private:
    using Base_t                      = Bordered<Wrapped_t>;
    static auto constexpr is_vertical = layout::is_vertical_v<Wrapped_t>;

   public:
    using Parameters = typename Base_t::Parameters;

   public:
    template <typename... Args>
    Passive(Args&&... args) : Base_t{std::forward<Args>(args)...}
    {
        auto const [h, w] =
            adjust_size_policies(Base_t::wrapped, this->Base_t::segments());
        this->set_policies(h, w);
    }

    Passive(Parameters p) : Base_t{std::move(p)}
    {
        auto const [h, w] =
            adjust_size_policies(Base_t::wrapped, this->Base_t::segments());
        this->set_policies(h, w);
    }

   protected:
    auto child_added_event(ox::Widget& child) -> bool override
    {
        auto const [h, w] =
            adjust_size_policies(Base_t::wrapped, this->Base_t::segments());
        this->set_policies(h, w);
        return Base_t::child_added_event(child);
    }

    auto child_removed_event(ox::Widget& child) -> bool override
    {
        auto const [h, w] =
            adjust_size_policies(Base_t::wrapped, this->Base_t::segments());
        this->set_policies(h, w);
        return Base_t::child_removed_event(child);
    }

    auto child_polished_event(ox::Widget& child) -> bool override
    {
        auto const [h, w] =
            adjust_size_policies(Base_t::wrapped, this->Base_t::segments());
        this->set_policies(h, w);
        return Base_t::child_polished_event(child);
    }

   private:
    void set_policies(Size_policy height, Size_policy width)
    {
        this->height_policy = height;
        this->width_policy  = width;
    }

    /// Return new size policies, order is [h, w].
    static auto adjust_size_policies(Wrapped_t const& wrapped,
                                     border::Segments s)
        -> std::pair<Size_policy, Size_policy>
    {
        auto get_hint = [](Widget const& child) {
            if constexpr (is_vertical)
                return child.height_policy.hint();
            else
                return child.width_policy.hint();
        };

        auto sum                = 0;
        auto const children     = wrapped.get_children();
        auto const begin_offset = wrapped.get_child_offset();
        for (auto i = begin_offset; i < children.size(); ++i)
            sum += get_hint(children[i]);

        if constexpr (is_vertical) {
            sum += s.north.has_value();
            sum += s.south.has_value();
            auto h = Size_policy{};
            h.fixed(sum);
            return {h, wrapped.width_policy};
        }
        else {
            sum += s.west.has_value();
            sum += s.east.has_value();
            auto w = Size_policy{};
            w.fixed(sum);
            return {wrapped.height_policy, w};
        }
    }
};

template <typename Widget_t>
class Passive<
    Bordered<Widget_t>,
    std::enable_if_t<!detail::is_horizontal_or_vertical_v<Widget_t>, void>>
    : public Bordered<Widget_t> {
   private:
    using Base_t = Bordered<Widget_t>;

   public:
    using Parameters = typename Base_t::Parameters;

   public:
    template <typename... Args>
    Passive(Args&&... args) : Base_t{std::forward<Args>(args)...}
    {
        auto const [h, w] =
            adjust_size_policies(Base_t::wrapped, this->Base_t::segments());
        this->set_policies(h, w);
    }

    Passive(Parameters p) : Base_t{std::move(p)}
    {
        auto const [h, w] =
            adjust_size_policies(Base_t::wrapped, this->Base_t::segments());
        this->set_policies(h, w);
    }

   protected:
    auto child_added_event(ox::Widget& child) -> bool override
    {
        auto const [h, w] =
            adjust_size_policies(Base_t::wrapped, this->Base_t::segments());
        this->set_policies(h, w);
        return Base_t::child_added_event(child);
    }

    auto child_removed_event(ox::Widget& child) -> bool override
    {
        auto const [h, w] =
            adjust_size_policies(Base_t::wrapped, this->Base_t::segments());
        this->set_policies(h, w);
        return Base_t::child_removed_event(child);
    }

    auto child_polished_event(ox::Widget& child) -> bool override
    {
        auto const [h, w] =
            adjust_size_policies(Base_t::wrapped, this->Base_t::segments());
        this->set_policies(h, w);
        return Base_t::child_polished_event(child);
    }

   private:
    void set_policies(Size_policy height, Size_policy width)
    {
        this->height_policy = height;
        this->width_policy  = width;
    }

    /// Returns [height, width]
    [[nodiscard]] static auto adjust_size_policies(Widget_t const& wrapped,
                                                   border::Segments s)
        -> std::array<ox::Size_policy, 2>
    {
        auto const border_height = s.north.has_value() + s.south.has_value();
        auto const border_width  = s.west.has_value() + s.east.has_value();

        auto hp = wrapped.height_policy;
        auto wp = wrapped.width_policy;

        hp.hint(hp.hint() + border_height);
        hp.min(hp.min() + border_height);
        // Overflow Check
        if (hp.max() <= Size_policy::maximum_max - border_height)
            hp.max(hp.max() + border_height);

        wp.hint(wp.hint() + border_width);
        wp.min(wp.min() + border_width);
        // Overflow Check
        if (wp.max() <= Size_policy::maximum_max - border_width)
            wp.max(wp.max() + border_width);

        return {hp, wp};
    }
};

/// Helper function to create a Passive instance.
template <typename Widget_t, typename... Args>
[[nodiscard]] auto passive(Args&&... args) -> std::unique_ptr<Passive<Widget_t>>
{
    return std::make_unique<Passive<Widget_t>>(std::forward<Args>(args)...);
}

/// Helper function to create a Passive instance.
template <typename Widget_t>
[[nodiscard]] auto passive(typename Passive<Widget_t>::Parameters p)
    -> std::unique_ptr<Passive<Widget_t>>
{
    return std::make_unique<Passive<Widget_t>>(std::move(p));
}

}  // namespace ox
#endif  // TERMOX_WIDGET_LAYOUTS_PASSIVE_HPP
