#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

#include <ox/ox.hpp>

#include "bmp_reader.hpp"

[[nodiscard]]
auto to_memory(std::filesystem::path const& filepath) -> std::vector<std::byte>
{
    auto file = std::ifstream{filepath, std::ios::binary};
    if (!file) { throw std::runtime_error{"Failed to open file"}; }
    auto vec = std::vector<char>{std::istreambuf_iterator<char>{file}, {}};
    auto const bytes = std::as_bytes(std::span{vec});
    return {bytes.begin(), bytes.end()};
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: TermOx.examples.bmp_view <filename>" << std::endl;
        return 1;
    }

    try {
        auto [size, pixels] = example::parse_bmp(to_memory(argv[1]));
        auto head = ox::PixelGrid{size, std::move(pixels)};

        return ox::Application{head}.run();
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown error" << std::endl;
        return 1;
    }
}