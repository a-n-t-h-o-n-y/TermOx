#include <zzz/test.hpp>

#include <array>
#include <limits>

#include "../examples/bmp_reader.hpp"

namespace {
void put(std::vector<std::byte>& bytes,
         std::size_t at,
         std::uint32_t value,
         int count = 4)
{
    for (int i = 0; i < count; ++i) {
        bytes.at(at + i) = (std::byte)(value >> (i * 8));
    }
}
auto bitmap(int width, int height, int depth, std::size_t offset = 54)
    -> std::vector<std::byte>
{
    auto const stride = ((std::size_t)width * depth + 31) / 32 * 4;
    auto bytes = std::vector<std::byte>(offset + stride * std::abs(height));
    bytes[0] = (std::byte)'B';
    bytes[1] = (std::byte)'M';
    put(bytes, 2, bytes.size());
    put(bytes, 10, offset);
    put(bytes, 14, 40);
    put(bytes, 18, width);
    put(bytes, 22, height);
    put(bytes, 26, 1, 2);
    put(bytes, 28, depth, 2);
    return bytes;
}
}  // namespace

TEST(bmp_direct_color_decoding_handles_padding_and_both_row_orders)
{
    for (auto depth : {24, 32}) {
        for (auto height : {2, -2}) {
            auto bytes = bitmap(1, height, depth);
            put(bytes, 54, 0xff0000, 3);
            put(bytes, 58, 0x0000ff, 3);
            auto [size, colors] = example::parse_bmp(bytes);
            ASSERT((size == ox::Area{1, 2}));
            ASSERT(colors[0] == ox::RGB{height > 0 ? 0x0000ffu : 0xff0000u});
            ASSERT(colors[1] == ox::RGB{height > 0 ? 0xff0000u : 0x0000ffu});
        }
    }
}

TEST(bmp_indexed_decoding_handles_bits_nibbles_and_bytes)
{
    for (auto depth : {1, 4, 8}) {
        auto bytes = bitmap(2, 1, depth, 62);
        put(bytes, 54, 0x123456);
        put(bytes, 58, 0xabcdef);
        put(bytes, 62, depth == 1 ? 0x40 : depth == 4 ? 0x01 : 0x0100, 2);
        auto [size, colors] = example::parse_bmp(bytes);
        ASSERT((size == ox::Area{2, 1}));
        ASSERT(colors[0] == ox::RGB{0x123456});
        ASSERT(colors[1] == ox::RGB{0xabcdef});
    }
}

TEST(bmp_bitfields_decode_default_555_565_and_32_bit_high_masks)
{
    auto bytes = bitmap(3, 1, 16);
    put(bytes, 54, 0x7c00, 2);
    put(bytes, 56, 0x03e0, 2);
    put(bytes, 58, 0x001f, 2);
    auto colors = example::parse_bmp(bytes).second;
    ASSERT(colors[0] == ox::RGB{0xff0000});
    ASSERT(colors[1] == ox::RGB{0x00ff00});
    ASSERT(colors[2] == ox::RGB{0x0000ff});
    bytes = bitmap(2, 1, 16, 66);
    put(bytes, 30, 3);
    put(bytes, 54, 0xf800);
    put(bytes, 58, 0x07e0);
    put(bytes, 62, 0x001f);
    put(bytes, 66, 0x8400, 2);  // Round intermediate 5/6-bit values to RGB8.
    put(bytes, 68, 0x001f, 2);
    colors = example::parse_bmp(bytes).second;
    ASSERT(colors[0] == ox::RGB{0x848200});
    ASSERT(colors[1] == ox::RGB{0x0000ff});
    bytes = bitmap(1, 1, 32, 66);
    put(bytes, 30, 3);
    put(bytes, 54, 0xff000000);
    put(bytes, 58, 0x00ff0000);
    put(bytes, 62, 0x0000ff00);
    put(bytes, 66, 0x12345600);
    ASSERT(example::parse_bmp(bytes).second[0] == ox::RGB{0x123456});
}

TEST(bmp_invalid_headers_and_every_truncated_prefix_are_rejected)
{
    auto valid = bitmap(1, 2, 24);
    for (std::size_t n = 0; n < valid.size(); ++n) {
        ASSERT_THROWS(example::parse_bmp(std::span{valid}.first(n)),
                      std::runtime_error);
    }
    for (auto [offset, value] :
         std::array{std::pair{0, 0u}, std::pair{10, 0u}, std::pair{10, 1000u},
                    std::pair{14, 39u}, std::pair{14, 1000u}, std::pair{18, 0u},
                    std::pair{18, 0xffffffffu}, std::pair{22, 0u},
                    std::pair{22, 0x80000000u}, std::pair{26, 2u}, std::pair{28, 2u},
                    std::pair{30, 1u}, std::pair{30, 3u}}) {
        auto bytes = valid;
        put(bytes, offset, value, offset == 26 || offset == 28 ? 2 : 4);
        ASSERT_THROWS(example::parse_bmp(bytes), std::runtime_error);
    }
}

TEST(bmp_invalid_masks_and_missing_palette_entries_are_rejected)
{
    auto valid = bitmap(1, 1, 16, 66);
    put(valid, 30, 3);
    put(valid, 54, 0xf800);
    put(valid, 58, 0x07e0);
    put(valid, 62, 0x001f);
    for (auto mask : {0u, 0x5000u, 0x10000u, 0x07e0u}) {
        auto bytes = valid;
        put(bytes, 54, mask);
        ASSERT_THROWS(example::parse_bmp(bytes), std::runtime_error);
    }
    for (auto depth : {1, 4, 8}) {
        auto bytes = bitmap(1, 1, depth, 58);
        put(bytes, 58, depth == 1 ? 0x80 : depth == 4 ? 0x10 : 0x01, 1);
        ASSERT_THROWS(example::parse_bmp(bytes), std::runtime_error);
        bytes = bitmap(1, 1, depth);
        ASSERT_THROWS(example::parse_bmp(bytes), std::runtime_error);
    }
}
