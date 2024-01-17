#ifndef TERMOX_DEMOS_GAME_OF_LIFE_FILETYPE_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_FILETYPE_HPP
#include <string>

namespace gol {

enum class FileType { Life_1_05, Life_1_06, Plaintext, RLE, Unknown };

/// Parses the file header and extension to retrieve FileType.
auto get_filetype(const std::string& filename) -> FileType;

/// Return the extension of the filename, without the period.
auto get_extension(const std::string& filename) -> std::string;

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_FILETYPE_HPP
