#include <termox/common/mb_to_u32.hpp>

#include <cstddef>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#ifdef __APPLE__
#    include <cwchar>
#else
#    include <cuchar>
#endif

namespace ox {

auto mb_to_u32(std::string_view sv) -> std::u32string
{
    auto mb_state  = std::mbstate_t{};
    auto n         = sv.size();
    char const* in = sv.data();
    auto buffer    = std::vector<char32_t>(sv.size() + 1, U'\0');
    char32_t* out  = buffer.data();

    while (n != 0) {
#ifdef __APPLE__
        static_assert(sizeof(wchar_t) == sizeof(char32_t));
        static_assert(alignof(wchar_t) == alignof(char32_t));
        auto const bytes_read =
            std::mbrtowc(reinterpret_cast<wchar_t*>(out), in, n, &mb_state);
#else
        auto const bytes_read = std::mbrtoc32(out, in, n, &mb_state);
#endif
        switch (bytes_read) {
            case std::size_t(-1):
            case std::size_t(-2):
            case std::size_t(-3):
                throw std::runtime_error{"mb_to_u32(): Bad Byte Sequence"};
            case 0:  // Null Byte Read
                (++out, ++in, --n);
                break;
            default: (++out, in += bytes_read, n -= bytes_read); break;
        }
    }
    return buffer.data() == nullptr ? U"" : std::u32string{buffer.data()};
}

}  // namespace ox
