#include <termox/common.hpp>

#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#ifdef __APPLE__
#    include <cwchar>
#else
#    include <cuchar>
#endif

#include <esc/detail/u32_to_mb.hpp>

namespace ox {

auto u32_to_mb(char32_t c) -> std::string
{
    auto [count, chars] = ::esc::detail::u32_to_mb(c);
    return std::string(chars.data(), count);
}

auto u32_to_mb(std::u32string_view sv) -> std::string
{
    auto buffer   = std::vector<char>(sv.size() * 4 + 1, '\0');
    auto mb_state = std::mbstate_t{};
    char* out     = buffer.data();

    for (char32_t c : sv) {
#ifdef __APPLE__
        static_assert(sizeof(wchar_t) == sizeof(char32_t));
        static_assert(alignof(wchar_t) == alignof(char32_t));
        auto const bytes_written =
            std::wcrtomb(out, static_cast<wchar_t>(c), &mb_state);
#else
        auto const bytes_written = std::c32rtomb(out, c, &mb_state);
#endif
        if (bytes_written == std::size_t(-1)) {
            throw std::runtime_error{"u32_to_mb(): Error."};
        }
        out += bytes_written;
    }
    return buffer.data() == nullptr ? "" : std::string{buffer.data()};
}

}  // namespace ox