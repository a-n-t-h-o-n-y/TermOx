#pragma once

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <ox/ox.hpp>
#include "bmp_reader.hpp"

namespace example {
// Resample only visible terminal cells; zoom never allocates a scaled image.
class BmpImage : public ox::Widget {
   public:
    using Widget::handle;
    BmpImage() : Widget{ox::FocusPolicy::Strong} {}
    auto measure(ox::Area, esc::WidthPolicy = {}) const -> ox::Area override
    { return {40, 10}; }

    void load(std::filesystem::path const& path)
    {
        auto file = std::ifstream{path, std::ios::binary | std::ios::ate};
        if (!file) throw std::runtime_error{"Could not open BMP file"};
        auto length = file.tellg();
        if (length < 54 || length > 32 * 1024 * 1024)
            throw std::runtime_error{"BMP must be between 54 bytes and 32 MiB"};
        auto bytes = std::vector<std::byte>((std::size_t)length);
        file.seekg(0);
        if (!file.read(reinterpret_cast<char*>(bytes.data()), length))
            throw std::runtime_error{"Could not read BMP file"};
        auto u32 = [&](int at) {
            auto const* p = reinterpret_cast<unsigned char const*>(bytes.data()) + at;
            return std::uint32_t(p[0]) | (std::uint32_t(p[1]) << 8) |
                   (std::uint32_t(p[2]) << 16) | (std::uint32_t(p[3]) << 24);
        };
        auto width = std::bit_cast<std::int32_t>(u32(18));
        auto height = (std::int64_t)std::bit_cast<std::int32_t>(u32(22));
        if (width <= 0 || height == 0 ||
            (std::uint64_t)width * (std::uint64_t)std::abs(height) > 16 * 1024 * 1024)
            throw std::runtime_error{"BMP exceeds the 16 megapixel limit"};
        // Commit only after decoding succeeds, retaining the previous image on error.
        auto [extent, pixels] = parse_bmp(bytes);
        auto name = path.filename().string();
        extent_ = extent;
        pixels_ = std::move(pixels);
        name_ = std::move(name);
        bytes_ = bytes.size();
        depth_ = std::to_integer<int>(bytes[28]) |
                 (std::to_integer<int>(bytes[29]) << 8);
        fit();
    }
    auto pixel_size() const -> ox::Area { return extent_; }
    auto offset() const -> ox::Point { return offset_; }
    auto zoom_percent() const -> int { return (int)std::lround(scale() * 100); }
    auto is_fit() const -> bool { return fit_; }
    auto description() const -> std::string
    {
        return name_ + " | " + std::to_string(extent_.width) + " x " +
               std::to_string(extent_.height) + " px | " +
               std::to_string(depth_) + "-bit | " + std::to_string(bytes_) + " bytes";
    }
    void fit() { fit_ = true; offset_ = {}; request_repaint(); }
    void zoom(double factor)
    {
        auto before = scale();
        auto left = std::max(0., (size().width - extent_.width * before) / 2.);
        auto top = std::max(0., (size().height * 2. - extent_.height * before) / 2.);
        auto center_x = (offset_.x + size().width / 2. - left) / before;
        auto center_y = (offset_.y + size().height - top) / before;
        zoom_ = std::clamp(factor, .01, 8.);
        fit_ = false;
        offset_ = {(int)std::lround(center_x * zoom_ - size().width / 2.),
                   (int)std::lround(center_y * zoom_ - size().height)};
        clamp_offset();
        request_repaint();
    }
    void zoom_by(double factor) { zoom(scale() * factor); }
    auto pan(ox::Point delta) -> bool
    {
        auto old = offset_;
        offset_.x += delta.x;
        offset_.y += delta.y;
        clamp_offset();
        request_repaint();
        return offset_ != old;
    }
    auto paint(ox::Canvas c) -> ox::Cursor override
    {
        ox::fill(c, appearance({}));
        if (pixels_.empty()) return {};
        auto s = scale();
        auto w = std::max(1, (int)std::lround(extent_.width * s));
        auto h = std::max(1, (int)std::lround(extent_.height * s));
        auto left = std::max(0, (size().width - w) / 2);
        auto top = std::max(0, (size().height * 2 - h) / 2);
        auto sample = [&](int x, int y) -> ox::Color {
            auto px = x + offset_.x - left;
            auto py = y + offset_.y - top;
            if (px < 0 || py < 0 || px >= w || py >= h)
                return appearance({}).background;
            auto sx = std::min(extent_.width - 1, (int)(px / s));
            auto sy = std::min(extent_.height - 1, (int)(py / s));
            if (s < 1.) {
                // Average a small grid across the source footprint to reduce aliasing.
                auto samples = std::min(4, (int)std::ceil(1. / s));
                unsigned red = 0, green = 0, blue = 0;
                for (int yy = 0; yy < samples; ++yy)
                    for (int xx = 0; xx < samples; ++xx) {
                        auto ax = std::min(extent_.width - 1,
                            (int)((px + (xx + .5) / samples) / s));
                        auto ay = std::min(extent_.height - 1,
                            (int)((py + (yy + .5) / samples) / s));
                        auto rgb = pixels_[(std::size_t)ay * extent_.width + ax];
                        red += rgb.red; green += rgb.green; blue += rgb.blue;
                    }
                auto count = (unsigned)(samples * samples);
                return ox::RGB{(std::uint8_t)(red / count),
                               (std::uint8_t)(green / count),
                               (std::uint8_t)(blue / count)};
            }
            return pixels_[(std::size_t)sy * extent_.width + sx];
        };
        auto clip = c.visible();
        for (int y = clip.at.y; y < clip.at.y + clip.size.height; ++y)
            for (int x = clip.at.x; x < clip.at.x + clip.size.width; ++x)
                c.write({x, y}, U"▄", {.background = sample(x, y * 2),
                                       .foreground = sample(x, y * 2 + 1)});
        return {};
    }
    auto handle(ox::KeyEvent const& e) -> ox::EventResponse override
    {
        if (e.action == ox::KeyAction::Release) return {};
        auto matches = [&](ox::KeyCode k) { return e.matches(k, {}, e.action); };
        auto delta = ox::Point{};
        if (matches(ox::KeyCode::ArrowLeft)) delta.x = -8;
        else if (matches(ox::KeyCode::ArrowRight)) delta.x = 8;
        else if (matches(ox::KeyCode::ArrowUp)) delta.y = -8;
        else if (matches(ox::KeyCode::ArrowDown)) delta.y = 8;
        else return {};
        pan(delta);
        return {.handled = true, .repaint = true};
    }
    auto handle(ox::MouseWheel const& e) -> ox::EventResponse override
    {
        auto changed = pan({e.dx * 4, e.dy * 4});
        return {.handled = changed, .repaint = changed};
    }

   protected:
    void on_arrange(ox::Rect) override { clamp_offset(); }

   private:
    ox::Area extent_{};
    std::vector<ox::RGB> pixels_;
    std::string name_;
    std::size_t bytes_ = 0;
    int depth_ = 0;
    bool fit_ = true;
    double zoom_ = 1.;
    ox::Point offset_{}; // Offset in scaled image pixels, including vertical half-cells.
    auto scale() const -> double
    {
        if (!fit_ || extent_.width == 0 || extent_.height == 0) return zoom_;
        return std::max(.000001, std::min((double)size().width / extent_.width,
                                         size().height * 2. / extent_.height));
    }
    void clamp_offset()
    {
        auto s = scale();
        offset_.x = std::clamp(offset_.x, 0, std::max(0, (int)std::lround(extent_.width * s) - size().width));
        offset_.y = std::clamp(offset_.y, 0, std::max(0, (int)std::lround(extent_.height * s) - size().height * 2));
    }
};
} // namespace example
