#define TEST_MAIN
#include <zzz/test.hpp>
#include "../examples/explorer_model.hpp"

namespace {
struct Fixture {
    std::filesystem::path path;
    Fixture() {
        auto base = std::filesystem::temp_directory_path();
        auto seed = std::chrono::steady_clock::now().time_since_epoch().count();
        for (int n = 0;; ++n) {
            path = base / ("termox-explorer-" + std::to_string(seed) + "-" + std::to_string(n));
            if (std::filesystem::create_directory(path)) break;
        }
    }
    ~Fixture() { std::error_code error; std::filesystem::remove_all(path, error); }
    void write(std::string const& name, std::string const& data) {
        std::ofstream{path / name, std::ios::binary} << data;
    }
};
}
TEST(explorer_sorts_numeric_sizes_and_keeps_real_paths_through_filtering)
{
    auto fixture = Fixture{};
    std::filesystem::create_directory(fixture.path / "folder");
    fixture.write("ten.txt", std::string(10, 'x'));
    fixture.write("two.txt", "xx");
    fixture.write(".hidden", "hidden");
    fixture.write("line\nbreak", "text");
    auto listing = explorer::read_directory(fixture.path, [] { return false; });
    auto rows = explorer::visible_rows(listing, ".TXT", false, 1, true);
    ASSERT(rows.size() == 2);
    ASSERT(listing.entries[rows[0]].path.filename() == "two.txt");
    ASSERT(listing.entries[rows[1]].path.filename() == "ten.txt");
    rows = explorer::visible_rows(listing, "", true, 1, false);
    ASSERT(listing.entries[rows[0]].directory);
    ASSERT(rows.size() == 5);
    auto visible = explorer::visible_rows(listing, "", false, 0, true);
    ASSERT(visible.size() == 4);
    auto found = std::find_if(listing.entries.begin(), listing.entries.end(),
                             [](auto const& entry) { return entry.path.filename() == "line\nbreak"; });
    ASSERT(found != listing.entries.end());
    ASSERT(found->name == "line\\x0abreak");
    ASSERT(explorer::read_preview(found->path).text == "text");
    ASSERT(explorer::read_directory(fixture.path, [] { return true; }).entries.empty());
}
TEST(explorer_preview_is_bounded_and_rejects_binary_and_bad_images)
{
    auto fixture = Fixture{};
    fixture.write("empty.txt", "");
    fixture.write("binary", std::string{"x\0y", 3});
    fixture.write("bad.bmp", "not a bitmap");
    fixture.write("large.txt", std::string(256 * 1024 - 1, 'x') + "界suffix");
    ASSERT(explorer::read_preview(fixture.path / "empty.txt").text == "(empty file)");
    ASSERT(explorer::read_preview(fixture.path / "binary").text.find("No text preview") == 0);
    auto prefix = explorer::read_preview(fixture.path / "large.txt").text;
    ASSERT(prefix.find("[Preview limited to 256 KiB]") != std::string::npos);
    ASSERT(prefix.size() < 256 * 1024 + 100);
    ASSERT(!esc::utf8_to_utf32(prefix).empty());
    bool rejected = false;
    try { explorer::read_preview(fixture.path / "bad.bmp"); }
    catch (std::exception const&) { rejected = true; }
    ASSERT(rejected);
    auto image = explorer::read_preview(std::filesystem::path{__FILE__}.parent_path() /
                                       "../examples/assets/julia.bmp");
    ASSERT(image.image.has_value());
    ASSERT((image.image->extent == ox::Area{640, 400}));
    ASSERT(image.image->pixels.size() == 640 * 400);
}
TEST(explorer_worker_replaces_pending_work_and_joins)
{
    auto mutex = std::mutex{};
    auto ready = std::condition_variable{};
    bool entered = false, release = false, done = false;
    auto calls = std::vector<int>{};
    {
        auto worker = explorer::Worker{};
        worker.submit([&](std::stop_token) {
            auto lock = std::unique_lock{mutex};
            entered = true; ready.notify_all();
            ready.wait_for(lock, std::chrono::seconds{5}, [&] { return release; });
        });
        {
            auto lock = std::unique_lock{mutex};
            if (!ready.wait_for(lock, std::chrono::seconds{5}, [&] { return entered; }))
                throw std::runtime_error{"Worker did not start"};
        }
        worker.submit([&](std::stop_token) { calls.push_back(1); });
        worker.submit([&](std::stop_token) {
            auto lock = std::lock_guard{mutex};
            calls.push_back(2); done = true; ready.notify_all();
        });
        {
            auto lock = std::unique_lock{mutex};
            release = true; ready.notify_all();
            if (!ready.wait_for(lock, std::chrono::seconds{5}, [&] { return done; }))
                throw std::runtime_error{"Worker did not finish"};
        }
    }
    ASSERT(calls == std::vector<int>{2});
}
