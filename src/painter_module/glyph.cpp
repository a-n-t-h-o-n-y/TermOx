#include <mcurses/painter_module/glyph.hpp>

#include <locale>
#include <codecvt>

namespace mcurses {

bool
Glyph::verify_length_(const std::string& s) {
	return 1 == std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>().from_bytes(s).size();
}

} // namespace mcurses