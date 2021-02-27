#ifndef TERMOX_WIDGET_ARRAY_HPP
#define TERMOX_WIDGET_ARRAY_HPP
#include <array>
#include <cstddef>
#include <functional>
#include <utility>

#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>

namespace ox {

// TODO move to layout folder.
/// Homogeneous collection of Widgets within a Layout_t<Widget_t>.
/** Depends on Layout_t::make_child(args...) to construct the Widget_t. */
template <typename Layout_t, std::size_t N>
class Array : public Layout_t {
   public:
    using Base = Layout_t;

   public:
    /// \p args will be copied into each Widget's constructor call.
    template <typename... Args>
    explicit Array(Args const&... args)
        : refs_{create_n_children(std::make_index_sequence<N>(), args...)}
    {}

    /// Get child by index.
    template <std::size_t Index>
    [[nodiscard]] auto get() -> auto&
    {
        return std::get<Index>(refs_).get();
    }

    /// Get child by index.
    template <std::size_t Index>
    [[nodiscard]] auto get() const -> auto const&
    {
        return std::get<Index>(refs_).get();
    }

   private:
    using References =
        std::array<std::reference_wrapper<typename Layout_t::Child_t>, N>;
    References refs_;

   private:
    template <std::size_t... Is, typename... Args>
    [[nodiscard]] auto create_n_children(std::index_sequence<Is...>,
                                         Args const&... args) -> References
    {
        return {this->insert_child(
            std::make_unique<typename Layout_t::Child_t>(args...), Is)...};
    }
};

// TODO make function() -> unique_ptr for each
template <typename Widget_t, std::size_t N>
using VArray = Array<layout::Vertical<Widget_t>, N>;

template <typename Widget_t, std::size_t N>
using HArray = Array<layout::Horizontal<Widget_t>, N>;

}  // namespace ox
#endif  // TERMOX_WIDGET_ARRAY_HPP
