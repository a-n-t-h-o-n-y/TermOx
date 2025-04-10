#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <ox/ox.hpp>

[[nodiscard]]
auto to_memory(std::filesystem::path const& filepath) -> std::vector<std::byte>
{
    auto file = std::ifstream{filepath, std::ios::binary};
    if (!file) { throw std::runtime_error{"Failed to open file"}; }
    auto vec = std::vector<char>{std::istreambuf_iterator<char>{file}, {}};
    return {reinterpret_cast<std::byte*>(vec.data()),
            reinterpret_cast<std::byte*>(vec.data() + vec.size())};
}

[[nodiscard]] auto parse_bmp(std::vector<std::byte> const& bytes)
    -> ox::PixelGrid::Matrix
{
    auto const u16 = [](auto p) { return std::uint16_t(p[0] | (p[1] << 8)); };
    auto const u32 = [](auto p) {
        return std::uint32_t(p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24));
    };
    auto const* data = reinterpret_cast<std::uint8_t const*>(bytes.data());
    if (data[0] != 'B' || data[1] != 'M') { throw std::runtime_error{"not a BMP"}; }

    auto const pixel_offset = u32(data + 10);
    auto const header_size = u32(data + 14);
    auto const width = static_cast<int>(u32(data + 18));
    auto const height = static_cast<int>(u32(data + 22));
    auto const bpp = u16(data + 28);
    auto const compression = u32(data + 30);

    // We'll handle only uncompressed or BI_BITFIELDS compression
    if (compression != 0 && compression != 3) {
        throw std::runtime_error{"unsupported BMP compression type"};
    }

    // Calculate palette offset and size
    auto const palette_offset = 14 + header_size;
    auto palette_size = 0;

    if (bpp <= 8) {
        // For 8-bit and below, palette size is either specified or 2^bpp
        palette_size = (pixel_offset - palette_offset) / 4;
        if (palette_size == 0) { palette_size = 1 << bpp; }
    }

    auto const palette = data + palette_offset;
    auto const pixels = data + pixel_offset;

    // Get color masks for 16/32 bit (default to standard masks if not specified)
    std::uint32_t red_mask = 0xFF0000;
    std::uint32_t green_mask = 0x00FF00;
    std::uint32_t blue_mask = 0x0000FF;

    if (compression == 3) {  // BI_BITFIELDS
        red_mask = u32(data + 54);
        green_mask = u32(data + 58);
        blue_mask = u32(data + 62);
    }
    else if (bpp == 16) {
        // Default 5-5-5 format
        red_mask = 0x7C00;
        green_mask = 0x03E0;
        blue_mask = 0x001F;
    }

    // Calculate bit positions
    auto bit_position = [](std::uint32_t mask) -> int {
        if (mask == 0) return 0;
        int pos = 0;
        while ((mask & 1) == 0) {
            mask >>= 1;
            pos++;
        }
        return pos;
    };

    auto bit_count = [](std::uint32_t mask) -> int {
        if (mask == 0) return 0;
        int count = 0;
        while (mask != 0) {
            if (mask & 1) count++;
            mask >>= 1;
        }
        return count;
    };

    auto const red_shift = bit_position(red_mask);
    auto const green_shift = bit_position(green_mask);
    auto const blue_shift = bit_position(blue_mask);

    auto const red_bits = bit_count(red_mask);
    auto const green_bits = bit_count(green_mask);
    auto const blue_bits = bit_count(blue_mask);

    // Calculate row stride (BMP rows are padded to 4-byte boundaries)
    auto const row_bytes = (width * bpp + 7) / 8;
    auto const row_stride = ((row_bytes + 3) / 4) * 4;

    auto out = ox::PixelGrid::Matrix{
        .area = {width, height},
        .pixels = std::vector<ox::TrueColor>(width * height, {0x000000}),
    };

    for (int y = 0; y < height; ++y) {
        // BMP stores rows bottom-to-top, so we need to invert the y-coordinate
        auto const src_row = pixels + (height - 1 - y) * row_stride;

        for (int x = 0; x < width; ++x) {
            std::uint8_t r = 0, g = 0, b = 0;

            if (bpp == 1) {
                // 1-bit BMP with palette (8 pixels per byte)
                auto const byte_offset = x / 8;
                auto const bit_offset = 7 - (x % 8);  // MSB first
                auto const idx = (src_row[byte_offset] >> bit_offset) & 0x01;
                b = palette[idx * 4 + 0];
                g = palette[idx * 4 + 1];
                r = palette[idx * 4 + 2];
            }
            else if (bpp == 4) {
                // 4-bit BMP with palette (2 pixels per byte)
                auto const byte_offset = x / 2;
                auto const nibble_offset = 1 - (x % 2);  // High nibble first
                auto const idx = (src_row[byte_offset] >> (nibble_offset * 4)) & 0x0F;
                b = palette[idx * 4 + 0];
                g = palette[idx * 4 + 1];
                r = palette[idx * 4 + 2];
            }
            else if (bpp == 8) {
                // 8-bit BMP with palette
                auto const idx = src_row[x];
                b = palette[idx * 4 + 0];
                g = palette[idx * 4 + 1];
                r = palette[idx * 4 + 2];
            }
            else if (bpp == 16) {
                // 16-bit BMP (typically 5-5-5 or 5-6-5 RGB format)
                auto const pixel_offset = x * 2;
                auto const pixel_value = u16(src_row + pixel_offset);

                // Extract components using masks and normalize to 8-bit
                auto extract_normalized = [](std::uint16_t val, std::uint32_t mask,
                                             int shift, int bits) -> std::uint8_t {
                    auto const max_val = (1 << bits) - 1;
                    auto const component = (val & mask) >> shift;
                    // Scale to 8-bit range
                    return static_cast<std::uint8_t>((component * 255 + max_val / 2) /
                                                     max_val);
                };

                r = extract_normalized(pixel_value, red_mask, red_shift, red_bits);
                g = extract_normalized(pixel_value, green_mask, green_shift,
                                       green_bits);
                b = extract_normalized(pixel_value, blue_mask, blue_shift, blue_bits);
            }
            else if (bpp == 24) {
                // 24-bit BMP with direct color values (BGR order)
                auto const pixel_offset = x * 3;
                b = src_row[pixel_offset + 0];
                g = src_row[pixel_offset + 1];
                r = src_row[pixel_offset + 2];
            }
            else if (bpp == 32) {
                // 32-bit BMP with direct color values (BGRA order typically)
                auto const pixel_offset = x * 4;
                auto const pixel_value = u32(src_row + pixel_offset);

                // Extract components using masks
                r = (std::uint8_t)((pixel_value & red_mask) >> red_shift);
                g = (std::uint8_t)((pixel_value & green_mask) >> green_shift);
                b = (std::uint8_t)((pixel_value & blue_mask) >> blue_shift);

                // Normalize if needed (if masks don't use 8 bits)
                if (red_bits != 8) {
                    r = static_cast<std::uint8_t>(
                        (r * 255 + ((1 << red_bits) - 1) / 2) / ((1 << red_bits) - 1));
                }
                if (green_bits != 8) {
                    g = static_cast<std::uint8_t>(
                        (g * 255 + ((1 << green_bits) - 1) / 2) /
                        ((1 << green_bits) - 1));
                }
                if (blue_bits != 8) {
                    b = static_cast<std::uint8_t>(
                        (b * 255 + ((1 << blue_bits) - 1) / 2) /
                        ((1 << blue_bits) - 1));
                }
            }
            else {
                throw std::runtime_error{"unsupported BMP bit depth: " +
                                         std::to_string(bpp)};
            }

            // Set the pixel
            out[{x, y}] = ox::RGB{(std::uint32_t)(r << 16) | (g << 8) | b};
        }
    }

    return out;
}

int main(int argc, char* argv[])
{
    if (argc < 2) {
        std::cerr << "Usage: TermOx.examples.bmp_view <filename>" << std::endl;
        return 1;
    }

    try {
        auto head = ox::PixelGrid{parse_bmp(to_memory(argv[1]))};

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