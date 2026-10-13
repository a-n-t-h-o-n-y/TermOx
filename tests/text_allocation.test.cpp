#define TEST_MAIN
#include <zzz/test.hpp>

#include <cstdlib>
#include <new>
#include <tuple>
#include <vector>

#include <ox/core/text.hpp>

namespace {
int remaining_allocations = -1;
}

// Fault injection is confined to this executable and enabled only around an edit.
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
