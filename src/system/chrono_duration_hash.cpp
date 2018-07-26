#include <cppurses/system/detail/chrono_duration_hash.hpp>

namespace std {
template <>
hash<std::chrono::duration<Tick, Ratio>>::result_type
hash<std::chrono::duration<Tick, Ratio>>::operator()(
    const argument_type& dur) const noexcept {
    return std::hash<Tick>{}(dur.count());
}
}  // namespace std
