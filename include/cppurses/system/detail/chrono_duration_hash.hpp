#ifndef CPPURSES_SYSTEM_DETAIL_CHRONO_DURATION_HASH_HPP
#define CPPURSES_SYSTEM_DETAIL_CHRONO_DURATION_HASH_HPP
#include <cstddef>
#include <functional>

#include <cppurses/system/detail/timer_event_loop.hpp>

namespace std {

/// Specialization of std::hash for duration type used within Timer_event_loop.
template <>
struct hash<cppurses::detail::Timer_event_loop::Period_t> {
    using argument_type = cppurses::detail::Timer_event_loop::Period_t;
    using result_type   = std::size_t;
    auto operator()(const argument_type& duration) const noexcept -> result_type
    {
        return std::hash<argument_type::rep>{}(duration.count());
    }
};

}  // namespace std
#endif  // CPPURSES_SYSTEM_DETAIL_CHRONO_DURATION_HASH_HPP
