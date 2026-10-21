#pragma once

#include <algorithm>
#include <condition_variable>
#include <ctime>
#include <iomanip>
#include <mutex>
#include <numeric>
#include <optional>
#include <sstream>
#include <stop_token>
#include <thread>
#include "bmp_image.hpp"

namespace explorer {
namespace fs = std::filesystem;

// Keep native paths for I/O. Escape names that cannot be displayed as terminal text.
inline auto display(std::string const& text) -> std::string
{
    try { ox::validate_row_text(text); return text; }
    catch (std::exception const&) {
        auto out = std::ostringstream{};
        for (unsigned char c : text) {
            if (c >= 32 && c < 127) out << c;
            else out << "\\x" << std::hex << std::setw(2) << std::setfill('0') << (int)c;
        }
        return out.str();
    }
}
inline auto folded(std::string text) -> std::string
{
    for (char& c : text) if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
    return text;
}
struct Entry {
    fs::path path;
    std::string name, type;
    bool directory = false;
    std::optional<std::uintmax_t> bytes = {};
    std::optional<fs::file_time_type> modified = {};
};
struct Directory {
    fs::path path;
    std::vector<Entry> entries;
    bool limited = false;
};
using Cancelled = std::function<bool()>;
inline auto read_directory(fs::path path, Cancelled cancelled) -> Directory
{
    path = fs::canonical(path);
    auto result = Directory{path, {}, false};
    for (auto const& item : fs::directory_iterator{path}) {
        if (cancelled()) return result;
        if (result.entries.size() == 10000) { result.limited = true; break; }
        auto error = std::error_code{};
        auto status = item.status(error); // Follow links for browsing, but retain their path.
        auto entry = Entry{item.path(), display(item.path().filename().string()), "Other"};
        entry.directory = !error && fs::is_directory(status);
        if (error) entry.type = "Unavailable";
        else if (entry.directory) entry.type = "Folder";
        else if (fs::is_regular_file(status)) {
            entry.type = "File";
            auto size = item.file_size(error);
            if (!error) entry.bytes = size;
        }
        auto link_error = std::error_code{};
        if (item.is_symlink(link_error)) entry.type += " (link)";
        auto modified = item.last_write_time(error);
        if (!error) entry.modified = modified;
        result.entries.push_back(std::move(entry));
    }
    return result;
}
inline auto visible_rows(Directory const& data, std::string query, bool hidden,
                         int column, bool ascending) -> std::vector<std::size_t>
{
    query = folded(query);
    auto rows = std::vector<std::size_t>{};
    for (std::size_t i = 0; i < data.entries.size(); ++i) {
        auto const& entry = data.entries[i];
        if (!hidden && entry.path.filename().string().starts_with('.')) continue;
        if (folded(entry.name).find(query) != std::string::npos) rows.push_back(i);
    }
    std::stable_sort(rows.begin(), rows.end(), [&](auto a, auto b) {
        auto const& x = data.entries[a]; auto const& y = data.entries[b];
        if (x.directory != y.directory) return x.directory; // Folders stay first.
        auto order = x.name <=> y.name;
        if (column == 1) order = x.bytes <=> y.bytes;
        if (column == 2) order = x.type <=> y.type;
        if (column == 3) order = x.modified <=> y.modified;
        if (order == 0) order = x.name <=> y.name;
        return ascending ? order < 0 : order > 0;
    });
    return rows;
}
inline auto timestamp(std::optional<fs::file_time_type> value) -> std::string
{
    if (!value) return "—";
    auto system_time = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        fs::file_time_type::clock::to_sys(*value));
    auto time = std::chrono::system_clock::to_time_t(system_time);
    auto result = std::ostringstream{};
    if (auto* utc = std::gmtime(&time)) result << std::put_time(utc, "%Y-%m-%d %H:%M");
    return result.str();
}
struct Preview {
    std::string text;
    std::optional<example::BmpData> image;
};
inline auto read_preview(fs::path const& path) -> Preview
{
    if (!fs::is_regular_file(path)) return {"No preview for this file type.", {}};
    if (folded(path.extension().string()) == ".bmp") return {{}, example::load_bmp(path)};
    auto file = std::ifstream{path, std::ios::binary};
    if (!file) throw std::runtime_error{"Could not read file"};
    constexpr std::size_t limit = 256 * 1024;
    auto text = std::string(limit + 1, '\0');
    file.read(text.data(), (std::streamsize)text.size());
    if (file.bad()) throw std::runtime_error{"Could not read file"};
    text.resize((std::size_t)file.gcount());
    auto truncated = text.size() > limit;
    if (truncated) text.resize(limit);
    // A bounded read may cut a UTF-8 scalar; trim only its incomplete trailing bytes.
    auto unicode = std::u32string{};
    for (int trim = 0;; ++trim) {
        try { unicode = esc::utf8_to_utf32(text); break; }
        catch (std::exception const&) {
            if (!truncated || trim == 3) return {"No text preview: binary or invalid UTF-8 data.", {}};
            text.pop_back();
        }
    }
    for (auto c : unicode)
        if ((c < 32 && c != '\n' && c != '\r' && c != '\t') || (c >= 127 && c < 160))
            return {"No text preview: binary or control data.", {}};
    if (truncated) text += "\n\n[Preview limited to 256 KiB]";
    return {text.empty() ? "(empty file)" : std::move(text), {}};
}

// One worker, with one pending request. New navigation replaces queued work.
// Declare after Application so it stops and joins before the application is destroyed.
class Worker {
   public:
    using Task = std::function<void(std::stop_token)>;
    Worker() : thread_{[this](std::stop_token stop) {
        while (!stop.stop_requested()) {
            auto task = Task{};
            {
                auto lock = std::unique_lock{mutex_};
                if (!ready_.wait(lock, stop, [&] { return pending_.has_value(); })) return;
                task = std::move(*pending_); pending_.reset();
            }
            task(stop);
        }
    }} {}
    void submit(Task task) {
        { auto lock = std::lock_guard{mutex_}; pending_ = std::move(task); }
        ready_.notify_one();
    }
   private:
    std::mutex mutex_;
    std::condition_variable_any ready_;
    std::optional<Task> pending_;
    std::jthread thread_;
};
} // namespace explorer
