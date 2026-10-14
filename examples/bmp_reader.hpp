#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

#include <ox/pixelgrid.hpp>

namespace example {
[[nodiscard]] inline auto parse_bmp(std::span<std::byte const> bytes)
    -> std::pair<ox::Area, std::vector<ox::RGB>>
{
    auto const u16 = [](auto p) { return std::uint16_t(p[0] | (p[1] << 8)); };
    auto const u32 = [](auto p) {
        return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
               (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
    };
    if (bytes.size() < 54) { throw std::runtime_error{"truncated BMP header"}; }
    auto const* data = reinterpret_cast<std::uint8_t const*>(bytes.data());
    if (data[0] != 'B' || data[1] != 'M') { throw std::runtime_error{"not a BMP"}; }

    auto const pixel_offset = u32(data + 10);
    auto const header_size = u32(data + 14);
    auto const width = std::bit_cast<std::int32_t>(u32(data + 18));
    auto const signed_height = std::bit_cast<std::int32_t>(u32(data + 22));
    auto const height64 =
        signed_height < 0 ? -(std::int64_t)signed_height : signed_height;
    if (width <= 0 || height64 <= 0 || height64 > std::numeric_limits<int>::max()) {
        throw std::runtime_error{"invalid BMP dimensions"};
    }
    auto const height = (int)height64;
    if (header_size < 40 || header_size > bytes.size() - 14 ||
        pixel_offset > bytes.size()) {
        throw std::runtime_error{"invalid BMP offsets"};
    }
    auto const bpp = u16(data + 28);
    auto const compression = u32(data + 30);
    if (u16(data + 26) != 1) { throw std::runtime_error{"invalid BMP plane count"}; }

    // We'll handle only uncompressed or BI_BITFIELDS compression
    if (compression != 0 && compression != 3) {
        throw std::runtime_error{"unsupported BMP compression type"};
    }

    if (bpp != 1 && bpp != 4 && bpp != 8 && bpp != 16 && bpp != 24 && bpp != 32) {
        throw std::runtime_error{"unsupported BMP bit depth"};
    }
    if (compression == 3 && bpp != 16 && bpp != 32) {
        throw std::runtime_error{"invalid BMP bitfields depth"};
    }
    auto const palette_offset = std::size_t{14} + header_size +
                                (compression == 3 && header_size == 40 ? 12 : 0);
    if (pixel_offset < palette_offset) {
        throw std::runtime_error{"overlapping BMP data"};
    }
    auto const palette_size = bpp <= 8 ? (pixel_offset - palette_offset) / 4 : 0;
    auto const palette = data + palette_offset;
    auto const pixels = data + pixel_offset;

    // Get color masks for 16/32 bit (default to standard masks if not specified)
    std::uint32_t red_mask = 0xFF0000;
    std::uint32_t green_mask = 0x00FF00;
    std::uint32_t blue_mask = 0x0000FF;

    if (compression == 3) {  // BI_BITFIELDS
        if (pixel_offset < 66 || (header_size != 40 && header_size < 52)) {
            throw std::runtime_error{"truncated BMP bitfields"};
        }
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

    auto validate_mask = [&](std::uint32_t mask) {
        if (mask == 0) { throw std::runtime_error{"empty BMP color mask"}; }
        auto const bits = std::popcount(mask);
        auto const shift = std::countr_zero(mask);
        if ((std::uint64_t{mask} >> shift) != (std::uint64_t{1} << bits) - 1 ||
            (bpp == 16 && mask > 0xffff)) {
            throw std::runtime_error{"invalid BMP color mask"};
        }
    };
    if (bpp == 16 || bpp == 32) {
        validate_mask(red_mask);
        validate_mask(green_mask);
        validate_mask(blue_mask);
        if ((red_mask & green_mask) || (red_mask & blue_mask) ||
            (green_mask & blue_mask)) {
            throw std::runtime_error{"overlapping BMP color masks"};
        }
    }
    auto extract = [](std::uint32_t value, std::uint32_t mask) -> std::uint8_t {
        auto const shift = std::countr_zero(mask);
        auto const maximum = std::uint64_t{mask} >> shift;
        return (
            std::uint8_t)(((std::uint64_t{value & mask} >> shift) * 255 + maximum / 2) /
                          maximum);
    };
    auto const row_bytes = (std::uint64_t{(unsigned)width} * bpp + 7) / 8;
    auto const stride64 = (row_bytes + 3) / 4 * 4;
    if (stride64 > std::numeric_limits<std::size_t>::max()) {
        throw std::runtime_error{"BMP row exceeds address range"};
    }
    auto const row_stride = (std::size_t)stride64;
    if ((std::size_t)height > (bytes.size() - pixel_offset) / row_stride) {
        throw std::runtime_error{"truncated BMP pixels"};
    }
    auto const size = ox::Area{width, height};
    auto out = std::vector<ox::RGB>(ox::pixel_count(size), ox::RGB{0x000000});

    for (int y = 0; y < height; ++y) {
        // BMP stores rows bottom-to-top, so we need to invert the y-coordinate
        auto const src_row =
            pixels + (std::size_t)(signed_height < 0 ? y : height - 1 - y) * row_stride;

        for (int x = 0; x < width; ++x) {
            std::uint8_t r = 0, g = 0, b = 0;

            if (bpp == 1) {
                // 1-bit BMP with palette (8 pixels per byte)
                auto const byte_offset = x / 8;
                auto const bit_offset = 7 - (x % 8);  // MSB first
                auto const idx = (src_row[byte_offset] >> bit_offset) & 0x01;
                if ((std::size_t)idx >= palette_size) {
                    throw std::runtime_error{"BMP palette index out of range"};
                }
                b = palette[idx * 4 + 0];
                g = palette[idx * 4 + 1];
                r = palette[idx * 4 + 2];
            }
            else if (bpp == 4) {
                // 4-bit BMP with palette (2 pixels per byte)
                auto const byte_offset = x / 2;
                auto const nibble_offset = 1 - (x % 2);  // High nibble first
                auto const idx = (src_row[byte_offset] >> (nibble_offset * 4)) & 0x0F;
                if ((std::size_t)idx >= palette_size) {
                    throw std::runtime_error{"BMP palette index out of range"};
                }
                b = palette[idx * 4 + 0];
                g = palette[idx * 4 + 1];
                r = palette[idx * 4 + 2];
            }
            else if (bpp == 8) {
                // 8-bit BMP with palette
                auto const idx = src_row[x];
                if ((std::size_t)idx >= palette_size) {
                    throw std::runtime_error{"BMP palette index out of range"};
                }
                b = palette[idx * 4 + 0];
                g = palette[idx * 4 + 1];
                r = palette[idx * 4 + 2];
            }
            else if (bpp == 16) {
                auto const value = u16(src_row + (std::size_t)x * 2);
                r = extract(value, red_mask);
                g = extract(value, green_mask);
                b = extract(value, blue_mask);
            }
            else if (bpp == 24) {
                // 24-bit BMP with direct color values (BGR order)
                auto const pixel_offset = (std::size_t)x * 3;
                b = src_row[pixel_offset + 0];
                g = src_row[pixel_offset + 1];
                r = src_row[pixel_offset + 2];
            }
            else if (bpp == 32) {
                auto const value = u32(src_row + (std::size_t)x * 4);
                r = extract(value, red_mask);
                g = extract(value, green_mask);
                b = extract(value, blue_mask);
            }

            // Set the pixel
            out[ox::pixel_index(size, {x, y})] =
                ox::RGB{(std::uint32_t)(r << 16) | (g << 8) | b};
        }
    }

    return {size, std::move(out)};
}

}  // namespace example
