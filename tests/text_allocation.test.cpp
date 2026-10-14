#define TEST_MAIN
#include <zzz/test.hpp>

#include <algorithm>
#include <array>
#include <cstdlib>
#include <new>
#include <tuple>
#include <vector>

#include <esc/layout.hpp>
#include <ox/core/renderer.hpp>
#include <ox/core/text.hpp>
#include <ox/datatable.hpp>
#include <ox/layout.hpp>

namespace {
int remaining_allocations = -1;
}

// Fault injection is confined to this executable and enabled only around the operation
// under test.
auto operator new(std::size_t bytes) -> void*
{
    if (remaining_allocations == 0) { throw std::bad_alloc{}; }
    if (remaining_allocations > 0) { --remaining_allocations; }
    auto* memory = std::malloc(bytes == 0 ? 1 : bytes);
    if (!memory) { throw std::bad_alloc{}; }
    return memory;
}
auto operator new[](std::size_t bytes) -> void* { return ::operator new(bytes); }
void operator delete(void* memory) noexcept { std::free(memory); }
void operator delete[](void* memory) noexcept { std::free(memory); }
void operator delete(void* memory, std::size_t) noexcept { std::free(memory); }
void operator delete[](void* memory, std::size_t) noexcept { std::free(memory); }

namespace {
auto document() -> std::string
{
    auto text = std::string{};
    for (auto i = 0; i < 200; ++i) {
        text += "line é界 👩‍💻\n";
    }
    return text;
}
auto state(ox::TextEdit const& edit)
{
    return std::tuple{
        edit.text(),
        std::vector<std::size_t>{edit.boundaries().begin(), edit.boundaries().end()},
        edit.anchor(),
        edit.caret(),
        edit.can_undo(),
        edit.can_redo()};
}
void check_history(ox::TextEdit actual, ox::TextEdit expected)
{
    for (;;) {
        auto const changed = expected.undo();
        ASSERT(actual.undo() == changed);
        ASSERT(state(actual) == state(expected));
        if (!changed) { break; }
    }
    for (;;) {
        auto const changed = expected.redo();
        ASSERT(actual.redo() == changed);
        ASSERT(state(actual) == state(expected));
        if (!changed) { break; }
    }
}
template <typename Factory, typename Action>
void check_failures(Factory factory, Action action)
{
    auto failures = 0;
    auto succeeded = false;
    for (auto budget = 0; budget < 64; ++budget) {
        auto original = factory();
        auto edit = original;
        auto const before = state(edit);
        remaining_allocations = budget;
        try {
            action(edit);
            remaining_allocations = -1;
            succeeded = true;
            break;
        }
        catch (std::bad_alloc const&) {
            remaining_allocations = -1;
            ++failures;
            ASSERT(state(edit) == before);
            check_history(edit, original);
        }
        catch (...) {
            remaining_allocations = -1;
            throw;
        }
    }
    ASSERT(failures > 0 && succeeded);
}
}  // namespace

TEST(text_replacement_allocation_failures_preserve_text_selection_and_history)
{
    check_failures(
        [] {
            auto edit = ox::TextEdit{document()};
            edit.insert("seed");
            edit.insert("next");
            edit.undo();
            edit.set_caret(edit.boundaries()[20]);
            edit.set_caret(edit.boundaries()[200], true);
            return edit;
        },
        [](ox::TextEdit& edit) { edit.insert("replacement\n界é👩‍💻"); });
}

TEST(text_undo_and_redo_allocation_failures_preserve_both_history_stacks)
{
    auto factory = [] {
        auto edit = ox::TextEdit{document()};
        edit.set_caret(edit.boundaries()[20]);
        edit.set_caret(edit.boundaries()[200], true);
        edit.insert("replacement\n界é👩‍💻");
        return edit;
    };
    check_failures(factory, [](ox::TextEdit& edit) { edit.undo(); });
    check_failures(
        [&] {
            auto edit = factory();
            edit.undo();
            return edit;
        },
        [](ox::TextEdit& edit) { edit.redo(); });
}

namespace {
auto layout_state(ox::TextLayout const& layout)
{
    auto positions = std::vector<ox::Point>{layout.position(0)};
    auto at = std::size_t{0};
    for (auto cluster : esc::graphemes(layout.text())) {
        at += cluster.size();
        positions.push_back(layout.position(at));
    }
    auto overrides = std::vector<std::pair<char32_t, int>>{};
    for (auto const& width : layout.policy().overrides) {
        overrides.emplace_back(width.scalar, width.cells);
    }
    auto const policy = layout.policy();
    return std::tuple{
        std::string{layout.text()},
        std::vector<ox::TextLine>{layout.lines().begin(), layout.lines().end()},
        positions,
        overrides,
        policy.ambiguous,
        policy.emoji,
        policy.clusters,
        policy.orphan_marks,
        policy.unassigned};
}
template <typename Action>
void check_layout_failures(Action action)
{
    auto original = ox::TextLayout{};
    original.update(document(), 40);
    auto const before = layout_state(original);
    auto failures = 0;
    auto succeeded = false;
    for (auto budget = 0; budget < 64; ++budget) {
        auto layout = original;
        auto const rows = layout.lines().data();
        auto const text = layout.text().data();
        remaining_allocations = budget;
        try {
            action(layout);
            remaining_allocations = -1;
            succeeded = true;
            break;
        }
        catch (std::bad_alloc const&) {
            remaining_allocations = -1;
            ++failures;
            ASSERT(layout_state(layout) == before);
            ASSERT(layout.lines().data() == rows && layout.text().data() == text);
            ASSERT(!layout.update(original.text(), 40));
            layout.update(original.text(), 41);
            ASSERT(std::ranges::equal(layout.lines(),
                                      ox::layout_text(original.text(), 41)));
        }
        catch (...) {
            remaining_allocations = -1;
            throw;
        }
    }
    ASSERT(failures > 0 && succeeded);
}
}  // namespace

TEST(text_layout_allocation_failures_preserve_analysis_policy_and_row_storage)
{
    check_layout_failures(
        [](ox::TextLayout& layout) { layout.update(layout.text(), 41); });
    auto const changed = document() + "new text\n";
    check_layout_failures([&](ox::TextLayout& layout) { layout.update(changed, 41); });
    auto middle = document();
    auto const newline = middle.find('\n', middle.size() / 2);
    middle.replace(newline, 1, " joined\nnew row\n");
    check_layout_failures([&](ox::TextLayout& layout) { layout.update(middle, 40); });
    auto overrides = std::array{esc::ScalarWidthOverride{U'e', 2}};
    check_layout_failures([&](ox::TextLayout& layout) {
        layout.update(layout.text(), 41, ox::TextWrap::Any, {.overrides = overrides});
    });
}

TEST(frame_resize_allocation_failures_preserve_cells_and_renderer_tracking)
{
    auto failures = 0;
    bool succeeded = false;
    for (int budget = 0; budget < 16; ++budget) {
        auto frame = ox::Frame{{4, 2}};
        frame.write({1, 1}, U"x");
        auto profile = esc::TerminalProfile{};
        profile.control = true;
        auto renderer = ox::Renderer{profile};
        renderer.render(frame);
        remaining_allocations = budget;
        try {
            frame.resize({8, 4});
            remaining_allocations = -1;
            succeeded = true;
            break;
        }
        catch (std::bad_alloc const&) {
            remaining_allocations = -1;
            ++failures;
            ASSERT((frame.size() == ox::Area{4, 2}));
            ASSERT((frame[ox::Point{1, 1}].text == U"x"));
            ASSERT(renderer.render(frame).empty());
            frame.write({1, 1}, U"y");
            ASSERT(!renderer.render(frame).empty());
        }
    }
    ASSERT(failures >= 3 && succeeded);
}

TEST(renderer_allocation_failures_invalidate_partial_row_tracking)
{
    auto failures = 0;
    bool succeeded = false;
    for (int budget = 0; budget < 64; ++budget) {
        auto profile = esc::TerminalProfile{};
        profile.control = true;
        auto renderer = ox::Renderer{profile};
        auto frame = ox::Frame{{4, 2}};
        renderer.render(frame);
        frame.resize({80, 24});
        frame.write({0, 0}, U"界");
        remaining_allocations = budget;
        try {
            renderer.render(frame);
            remaining_allocations = -1;
            succeeded = true;
            break;
        }
        catch (std::bad_alloc const&) {
            remaining_allocations = -1;
            ++failures;
            ASSERT(!renderer.render(frame).empty());
            ASSERT(renderer.render(frame).empty());
        }
    }
    ASSERT(failures >= 2 && succeeded);
}

TEST(table_sort_and_schema_allocation_failures_preserve_data_geometry_and_selection)
{
    for (auto schema : {false, true}) {
        auto failures = 0;
        auto succeeded = false;
        for (int budget = 0; budget < 64; ++budget) {
            auto table = ox::DataTable{{.columns = {{.heading = "a"}},
                                        .rows = {{"third"}, {"second"}, {"first"}}}};
            ox::arrange(table, {{}, {20, 4}});
            table.select(1);
            auto const rows = std::vector<std::vector<std::string>>{
                table.rows().begin(), table.rows().end()};
            auto const before = std::tuple{table.selected_row(), table.content_size(),
                                           table.scroll_offset()};
            remaining_allocations = budget;
            try {
                if (schema) {
                    table.set_data({{.heading = "replacement"}, {.heading = "next"}},
                                   {{"a", "b"}});
                }
                else {
                    table.sort_rows(0);
                }
                remaining_allocations = -1;
                succeeded = true;
                break;
            }
            catch (std::bad_alloc const&) {
                remaining_allocations = -1;
                ++failures;
                ASSERT(std::ranges::equal(table.rows(), rows));
                ASSERT(table.columns().size() == 1 &&
                       table.columns()[0].heading == "a");
                ASSERT((std::tuple{table.selected_row(), table.content_size(),
                                   table.scroll_offset()} == before));
            }
            catch (...) {
                remaining_allocations = -1;
                throw;
            }
        }
        ASSERT(failures > 0 && succeeded);
    }
}

TEST(
    layout_allocation_failures_preserve_child_geometry_and_warm_refresh_allocates_nothing)
{
    auto failures = 0;
    auto succeeded = false;
    for (int budget = 0; budget < 64; ++budget) {
        auto row = ox::Row<std::vector<ox::Widget>>{};
        for (int i = 0; i < 10; ++i) {
            row.children.emplace_back();
        }
        ox::arrange(row, {{}, {20, 3}});
        auto before = std::vector<ox::Rect>{};
        for (auto const& child : row.children) {
            before.push_back(child.bounds());
        }
        row.children.front().size_policy = ox::SizePolicy::fixed(1);
        remaining_allocations = budget;
        try {
            row.refresh();
            remaining_allocations = -1;
            succeeded = true;
            break;
        }
        catch (std::bad_alloc const&) {
            remaining_allocations = -1;
            ++failures;
            for (std::size_t i = 0; i < before.size(); ++i) {
                ASSERT(row.children[i].bounds() == before[i]);
            }
            row.refresh();
            ASSERT(row.children.front().size().width == 1);
        }
        catch (...) {
            remaining_allocations = -1;
            throw;
        }
    }
    ASSERT(failures > 0 && succeeded);
    auto row = ox::Row<std::vector<ox::Widget>>{};
    for (int i = 0; i < 10; ++i) {
        row.children.emplace_back();
    }
    ox::arrange(row, {{}, {20, 3}});
    remaining_allocations = 0;
    try {
        row.refresh();
    }
    catch (...) {
        remaining_allocations = -1;
        throw;
    }
    remaining_allocations = -1;
    ASSERT(row.children.back().at().x == 18);
}
