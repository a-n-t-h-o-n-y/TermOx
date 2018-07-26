#ifndef CPPURSES_SYSTEM_DETAIL_CHRONO_DURATION_HASH_HPP
#define CPPURSES_SYSTEM_DETAIL_CHRONO_DURATION_HASH_HPP
#include <functional>

/// Custom specialization of std::hash for std::chrono::duration.
namespace std {
template <typename Tick, typename Ratio>
struct hash<std::chrono::duration<Tick, Ratio>> {
    using argument_type = std::chrono::duration<Tick, Ratio>;
    using result_type = std::size_t;
    result_type operator()(const argument_type& dur) const noexcept;
};
}  // namespace std
#endif  // CPPURSES_SYSTEM_DETAIL_CHRONO_DURATION_HASH_HPP
