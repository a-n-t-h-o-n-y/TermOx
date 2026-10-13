#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <memory>
#include <new>
#include <string>
#include <vector>

#include <ox/ox.hpp>

namespace {
std::size_t heap_allocations = 0;
}

// Count ordinary C++ allocation calls in the single-threaded benchmark executable.
auto operator new(std::size_t bytes) -> void*
{
    auto* memory = std::malloc(bytes == 0 ? 1 : bytes);
    if (!memory) { throw std::bad_alloc{}; }
    ++heap_allocations;
    return memory;
}
auto operator new[](std::size_t bytes) -> void* { return ::operator new(bytes); }
void operator delete(void* memory) noexcept { std::free(memory); }
void operator delete[](void* memory) noexcept { std::free(memory); }
void operator delete(void* memory, std::size_t) noexcept { std::free(memory); }
void operator delete[](void* memory, std::size_t) noexcept { std::free(memory); }

namespace {
using Clock = std::chrono::steady_clock;
std::size_t checksum = 0;

template <typename Function>
void measure(std::string const& name, int iterations, Function work)
{
    for (auto i = 0; i < 10; ++i) {
        checksum += work(i);
    }
    auto samples = std::vector<double>{};
    auto const before = checksum;
    auto allocations = std::size_t{0};
    for (auto sample = 0; sample < 5; ++sample) {
        auto const start = Clock::now();
        auto const allocation_start = heap_allocations;
        for (auto i = 0; i < iterations; ++i) {
            checksum += work(i);
        }
        allocations += heap_allocations - allocation_start;
        auto const elapsed =
            std::chrono::duration<double, std::micro>(Clock::now() - start);
        samples.push_back(elapsed.count() / iterations);
    }
    std::ranges::sort(samples);
    std::cout << name << ',' << iterations << ',' << std::fixed << std::setprecision(3)
              << samples[2] << ',' << checksum - before << ','
              << (double)allocations / (iterations * 5) << '\n';
}

struct Tree : ox::Widget {
    std::vector<std::unique_ptr<ox::Widget>> children;
    auto get_children() -> zzz::Generator<ox::Widget&> override
    {
        for (auto& child : children) {
            co_yield *child;
        }
    }
};

void text_benchmarks()
{
    auto document = std::string{};
    for (auto i = 0; i < 1000; ++i) {
        document += "The quick brown fox jumps over the lazy dog. 界 é\tend\n";
    }
    auto layout = ox::TextLayout{};
    layout.update(document, 40);
    measure("text.position.1000-lines", 5000, [&](int i) {
        auto const row = (std::size_t)i % layout.lines().size();
        auto const point = layout.position(layout.lines()[row].end);
        return (std::size_t)(point.x + point.y);
    });
    measure("text.offset.1000-lines", 5000, [&](int i) {
        return layout.offset({i % 40, i % (int)layout.lines().size()});
    });
    auto wide = std::string(10000, 'x');
    layout.update(wide, 0, ox::TextWrap::None);
    measure("text.position.10000-columns", 1000, [&](int i) {
        return (std::size_t)layout.position((std::size_t)(i * 7919) % wide.size()).x;
    });
    measure("text.offset.10000-columns", 1000,
            [&](int i) { return layout.offset({(i * 7919) % (int)wide.size(), 0}); });
    measure("text.layout.rebuild", 100,
            [&](int i) { return (std::size_t)layout.update(document, 40 + i % 2); });
    measure("text.layout.unchanged", 5000,
            [&](int) { return (std::size_t)layout.update(document, 41); });
    auto editor = ox::TextEdit{document, true, 100};
    editor.set_caret(editor.boundaries()[editor.boundaries().size() / 2]);
    measure("text.edit.insert-erase", 100, [&](int) {
        editor.insert("x");
        editor.erase_backward();
        return editor.text().size();
    });
}

void editing_benchmarks()
{
    for (auto lines : {1000, 10000}) {
        auto document = std::string{};
        for (auto i = 0; i < lines; ++i) {
            document += "The quick brown fox jumps over the lazy dog. 界 é	end\n";
        }
        auto const iterations = lines == 1000 ? 100 : 25;
        auto const suffix = std::to_string(lines);
        for (auto position : {"begin", "middle", "end"}) {
            auto edit = ox::TextEdit{document};
            auto const offset = std::string_view{position} == "begin" ? 0
                                : std::string_view{position} == "end"
                                    ? edit.text().size()
                                    : edit.boundaries()[edit.boundaries().size() / 2];
            edit.set_caret(offset);
            measure("editing.insert-erase." + suffix + "." + position, iterations,
                    [&](int) {
                        edit.insert("x");
                        edit.erase_backward();
                        return edit.text().size() + edit.caret();
                    });
        }
        auto edit = ox::TextEdit{document};
        edit.set_caret(edit.boundaries()[edit.boundaries().size() / 2]);
        edit.insert("x");
        measure("editing.undo-redo." + suffix, iterations, [&](int) {
            edit.undo();
            edit.redo();
            return edit.text().size() + edit.caret();
        });
        edit.undo();
        auto const begin = edit.caret();
        auto end = begin;
        for (auto i = 0; i < 10; ++i) {
            end = document.find('\n', end) + 1;
        }
        edit.set_caret(end, true);
        measure("editing.selection." + suffix, iterations, [&](int) {
            edit.insert("replacement\n界é");
            edit.undo();
            return edit.text().size() + edit.caret() + edit.anchor();
        });
        edit.set_caret(begin);
        measure("editing.paste." + suffix, iterations, [&](int) {
            edit.paste({"paste\n\xf0\x9f", true, false});
            edit.paste({"\x99\x82", false, true});
            edit.undo();
            return edit.text().size() + edit.caret();
        });
    }
    auto line = ox::TextEdit{std::string(10000, 'x')};
    line.set_caret(5000);
    measure("editing.insert-erase.single-line", 100, [&](int) {
        line.insert("x");
        line.erase_backward();
        return line.text().size() + line.caret();
    });
    auto edit = ox::TextEdit{std::string(10000, 'x'), true, 0};
    edit.set_caret(5000);
    measure("editing.insert-erase.no-history", 100, [&](int) {
        edit.insert("x");
        edit.erase_backward();
        return edit.text().size() + edit.caret();
    });
}

void routing_benchmarks()
{
    for (auto count : {100, 1000}) {
        auto root = Tree{};
        root.size = {80, 24};
        for (auto i = 0; i < count; ++i) {
            auto child = std::make_unique<ox::Widget>(ox::FocusPolicy::Strong);
            child->size = {1, 1};
            child->at = {i % 80, i / 80};
            root.children.push_back(std::move(child));
        }
        auto app = ox::Application{root};
        measure("routing.idle." + std::to_string(count), 100,
                [&](int) { return (std::size_t)app.step().repaint; });
        measure("routing.key." + std::to_string(count), 100, [&](int) {
            return (std::size_t)app.handle_key_press({.key = esc::KeyCode::ArrowRight})
                .handled;
        });
        auto const point = root.children.back()->at;
        measure("routing.mouse." + std::to_string(count), 100, [&](int) {
            return (std::size_t)app.handle_mouse_move({.at = point, .held_button = {}})
                .handled;
        });
        measure("routing.tab." + std::to_string(count), 100, [&](int) {
            return (std::size_t)app.handle_key_press({.key = esc::KeyCode::Tab})
                .handled;
        });
        measure("routing.paste." + std::to_string(count), 100, [&](int) {
            return (std::size_t)app.handle_paste({"text", true, true}).handled;
        });
        measure("routing.reorder." + std::to_string(count), 100, [&](int) {
            std::rotate(root.children.begin(), root.children.begin() + 1,
                        root.children.end());
            return (std::size_t)app.step().repaint;
        });
    }
    auto root = Tree{};
    root.size = {80, 24};
    auto* parent = &root;
    for (auto i = 0; i < 32; ++i) {
        auto child = std::make_unique<Tree>();
        child->size = root.size;
        auto* next = child.get();
        parent->children.push_back(std::move(child));
        parent = next;
    }
    auto leaf = std::make_unique<ox::Widget>(ox::FocusPolicy::Strong);
    leaf->size = root.size;
    parent->children.push_back(std::move(leaf));
    for (auto i = 0; i < 1000; ++i) {
        root.children.push_back(std::make_unique<ox::Widget>());
    }
    auto app = ox::Application{root};
    measure("routing.key.depth32", 100, [&](int) {
        return (std::size_t)app.handle_key_press({.key = esc::KeyCode::ArrowRight})
            .handled;
    });
}

void renderer_benchmarks()
{
    auto profile = esc::TerminalProfile{};
    profile.control = true;
    profile.colors = esc::ColorSupport::TrueColor;
    profile.traits = ox::Trait::Bold;
    for (auto size : {ox::Area{80, 24}, ox::Area{200, 60}}) {
        auto frame = ox::Frame{size};
        auto renderer = ox::Renderer{profile};
        checksum += renderer.render(frame).size();
        auto const suffix =
            std::to_string(size.width) + "x" + std::to_string(size.height);
        measure("renderer.unchanged." + suffix, 1000,
                [&](int) { return renderer.render(frame).size(); });
        measure("renderer.single-cell." + suffix, 1000, [&](int i) {
            frame.write({size.width / 2, size.height / 2}, i % 2 ? U"x" : U"y");
            return renderer.render(frame).size();
        });
        measure("renderer.full-frame." + suffix, 100, [&](int i) {
            frame.clear({.traits = i % 2 ? ox::Trait::Bold : ox::Trait::None});
            return renderer.render(frame).size();
        });
    }
}
}  // namespace

int main(int argc, char** argv)
{
    auto const routing_only = argc == 2 && std::string_view{argv[1]} == "--routing";
    auto const editing_only = argc == 2 && std::string_view{argv[1]} == "--editing";
    if (argc > 1 && !routing_only && !editing_only) { return 2; }
    std::cout << "workload,iterations,median_us_per_operation,checksum,allocations_per_"
                 "operation\n";
    if (editing_only) { editing_benchmarks(); }
    else {
        if (!routing_only) { text_benchmarks(); }
        routing_benchmarks();
        if (!routing_only) { renderer_benchmarks(); }
    }
    std::cerr << "checksum=" << checksum << '\n';
}
