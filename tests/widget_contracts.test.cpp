#include <memory>
#include <ox/ox.hpp>
#include <zzz/test.hpp>
using namespace ox;

TEST(activation_is_once_per_press_and_mouse_capture_cancels_outside_release)
{
    auto button = Button{{.label = {.text = "run"}}};
    auto app = Application{button};
    app.dispatch(esc::Resize{{8, 1}});
    int actions = 0;
    button.on_activate.connect([&] { ++actions; });
    app.dispatch(KeyEvent{.key = U' '});
    app.dispatch(KeyEvent{.key = U' ', .action = KeyAction::Repeat});
    app.dispatch(KeyEvent{.key = KeyCode::Enter, .action = KeyAction::Release});
    ASSERT(actions == 1 && !button.is_pressed());
    app.dispatch(MousePress{{1, 0}, MouseButton::Left});
    ASSERT(button.is_pressed() && button.is_pointer_captured());
    app.dispatch(MouseMove{{20, 0}, MouseButton::Left});
    app.dispatch(MouseRelease{{20, 0}, MouseButton::Left});
    ASSERT(actions == 1 && !button.is_pointer_captured());
    app.dispatch(MousePress{{1, 0}, MouseButton::Left});
    app.dispatch(MouseMove{{20, 0}, MouseButton::Left});
    app.dispatch(MouseMove{{1, 0}, MouseButton::Left});
    app.dispatch(MouseRelease{{1, 0}, MouseButton::Left});
    ASSERT(actions == 2);
    auto box = CheckBox{{.label = {.text = "Enable"}}};
    box.handle(KeyEvent{.key = U' '});
    box.handle(KeyEvent{.key = U' ', .action = KeyAction::Repeat});
    ASSERT(box.checked());
}

TEST(pointer_capture_follows_root_moves_and_cancels_when_owner_is_disabled_or_removed)
{
    auto root = std::make_unique<Button>();
    auto app = Application{*root};
    app.dispatch(esc::Resize{{4, 1}});
    app.dispatch(MousePress{{0, 0}, MouseButton::Left});
    auto moved = std::move(*root);
    root.reset();
    ASSERT(moved.is_pointer_captured());
    app.dispatch(MouseRelease{{0, 0}, MouseButton::Left});
    ASSERT(!moved.is_pointer_captured());
    app.dispatch(MousePress{{0, 0}, MouseButton::Left});
    moved.enabled = false;
    app.step();
    ASSERT(!moved.is_pointer_captured() && !moved.is_pressed());
    auto stack = Stack{Button{}, Button{}};
    auto pages = Application{stack};
    pages.dispatch(esc::Resize{{4, 1}});
    pages.dispatch(MousePress{{0, 0}, MouseButton::Left});
    ASSERT(std::get<0>(stack.children).is_pointer_captured());
    stack.select(1);
    pages.step();
    ASSERT(!std::get<0>(stack.children).is_pressed());
    ASSERT(pages.focus().get() == track(std::get<1>(stack.children)));
}

TEST(signal_snapshots_preserve_mutable_slots_and_cancel_disconnected_entries)
{
    auto signal = sl::Signal<void()>{};
    auto seen = std::vector<int>{};
    signal.connect([count = 0, &seen]() mutable { seen.push_back(++count); });
    signal();
    signal();
    auto copy = signal;
    copy();
    copy();
    signal();
    ASSERT((seen == std::vector<int>{1, 2, 3, 4, 3}));
    auto changing = sl::Signal<void()>{};
    auto calls = std::vector<int>{};
    auto later = sl::Identifier{};
    auto once = true;
    changing.connect([&] {
        calls.push_back(1);
        if (std::exchange(once, false)) {
            changing.disconnect(later);
            changing.connect([&] { calls.push_back(3); });
        }
    });
    later = changing.connect([&] { calls.push_back(2); });
    changing();
    changing();
    ASSERT((calls == std::vector<int>{1, 1, 3}));
    auto owner = std::make_unique<sl::Signal<void()>>();
    auto* pointer = owner.get();
    int count = 0;
    owner->connect([&] {
        ++count;
        owner.reset();
    });
    owner->connect([&] { count += 10; });
    (*pointer)();
    ASSERT(count == 1);
}

TEST(text_commands_notify_once_keep_programmatic_edits_and_protect_readonly_input)
{
    auto line = LineEdit{{.text = "abc", .placeholder = "name", .read_only = true}};
    auto app = Application{line};
    app.dispatch(esc::Resize{{10, 1}});
    auto changes = std::vector<std::string>{};
    int selections = 0;
    line.on_change.connect([&](std::string const& value) { changes.push_back(value); });
    line.on_selection_change.connect([&](auto, auto) { ++selections; });
    app.dispatch(KeyEvent{.key = U'x'});
    app.dispatch(KeyEvent{.key = KeyCode::Delete});
    app.dispatch(esc::PasteChunk{.bytes = "paste", .first = true, .last = true});
    ASSERT(line.text() == "abc" && changes.empty());
    app.dispatch(KeyEvent{.key = KeyCode::ArrowRight});
    ASSERT(line.editor().caret() == 1 && selections == 1);
    ASSERT(line.set_text("next"));
    ASSERT(!line.set_text("next"));
    ASSERT((changes == std::vector<std::string>{"next"}));
    line.edit([](TextEdit& edit) {
        edit.select_all();
        edit.insert("界");
    });
    ASSERT(line.text() == "界" && changes.size() == 2);
    line.read_only = false;
    line.undo();
    ASSERT(line.text() == "next" && changes.size() == 3);
}

TEST(text_submission_validates_whole_values_without_rejecting_intermediate_edits)
{
    auto line =
        LineEdit{{.validator = [](std::string_view value) { return value == "12"; }}};
    int submissions = 0, invalid = 0;
    line.on_submit.connect([&](auto const&) { ++submissions; });
    line.on_invalid.connect([&](auto const&) { ++invalid; });
    line.insert("1");
    ASSERT(line.text() == "1");
    ASSERT(!line.submit() && invalid == 1);
    line.insert("2");
    ASSERT(line.submit() && submissions == 1);
    line.handle(KeyEvent{.key = KeyCode::Enter, .action = KeyAction::Repeat});
    ASSERT(submissions == 1);
}

TEST(clipboard_commands_use_the_host_service_and_readonly_copy_remains_available)
{
    auto line = LineEdit{{.text = "hello"}};
    auto app = Application{line};
    app.dispatch(esc::Resize{{12, 1}});
    line.select_all();
    ASSERT(line.copy() && app.clipboard.read() == "hello");
    ASSERT(line.cut() && line.text().empty());
    ASSERT(line.paste() && line.text() == "hello");
    line.read_only = true;
    line.select_all();
    ASSERT(line.copy() && !line.cut());
    app.clipboard.read = [] { return "host"; };
    line.read_only = false;
    ASSERT(line.paste() && line.text() == "host");
}

TEST(text_viewport_scroll_preserves_caret_selection_and_paint_emits_no_notifications)
{
    auto box = TextBox{{.text = "zero\none\ntwo\nthree\nfour", .wrap = TextWrap::None}};
    arrange(box, {{}, {8, 2}});
    auto caret = box.editor().caret();
    auto selection = box.editor().selection();
    int scrolls = 0;
    box.on_scroll.connect([&](Point, Area, Area) { ++scrolls; });
    ASSERT(box.set_scroll_offset({0, 3}));
    ASSERT(box.editor().caret() == caret && box.editor().selection() == selection);
    auto frame = Frame{{8, 2}};
    box.paint(Canvas{frame});
    box.paint(Canvas{frame});
    ASSERT(box.scroll_offset().y == 3 && scrolls == 1);
    box.handle(KeyEvent{.key = KeyCode::ArrowDown});
    ASSERT(box.editor().caret() != caret && box.scroll_offset().y < 3);
}

TEST(pointer_selection_and_scrollbar_drag_continue_outside_widget_bounds)
{
    auto line = LineEdit{{.text = "abcdef"}};
    auto app = Application{line};
    app.dispatch(esc::Resize{{6, 1}});
    app.dispatch(MousePress{{1, 0}, MouseButton::Left});
    app.dispatch(MouseMove{{30, 0}, MouseButton::Left});
    ASSERT((line.editor().selection() == std::pair<std::size_t, std::size_t>{1, 6}));
    app.dispatch(MouseRelease{{30, 0}, MouseButton::Left});
    ASSERT(!line.is_pointer_captured());
    auto bar = ScrollBar{{.content_length = 100, .viewport_length = 10}};
    auto bars = Application{bar};
    bars.dispatch(esc::Resize{{1, 10}});
    bars.dispatch(MousePress{{0, 0}, MouseButton::Left});
    bars.dispatch(MouseMove{{0, 50}, MouseButton::Left});
    ASSERT(bar.position() == 90 && bar.is_pointer_captured());
    bars.dispatch(MouseRelease{{0, 50}, MouseButton::Left});
    ASSERT(!bar.is_pointer_captured());
}

TEST(measured_layout_combines_axis_policies_gap_unicode_and_cross_alignment)
{
    auto row = Row{Label{"界"}, Button{{.label = {.text = "run"}}}};
    row.gap = 2;
    row.cross_align = CrossAlign::Center;
    arrange(row, {{}, {10, 3}});
    ASSERT((row.measure({10, 3}) == Area{7, 1}));
    ASSERT(std::get<0>(row.children).size().height == 1 &&
           std::get<0>(row.children).at().y == 1);
    ASSERT(std::get<1>(row.children).at().x ==
           std::get<0>(row.children).size().width + 2);
    auto column = Column{Label{"first"}, LineEdit{}, Divider{}};
    arrange(column, {{}, {10, 8}});
    ASSERT(std::get<0>(column.children).size().height == 1 &&
           std::get<1>(column.children).size().height == 1);
    ASSERT((std::get<2>(column.children).size() == Area{10, 1}));
    auto app = Application{row};
    app.dispatch(esc::Resize{{12, 3}});
    std::get<0>(row.children).set_text("longer");
    app.step();
    ASSERT(std::get<0>(row.children).size().width >= 6);
}

namespace {
struct Layers : Widget {
    Label child{"child"};
    std::vector<int>* order;
    explicit Layers(std::vector<int>& order) : order{&order} {}
    void on_arrange(Rect) override { arrange(child, {{}, size()}); }
    void paint_background(Canvas canvas) override
    {
        order->push_back(1);
        fill(canvas, U'.');
    }
    auto paint(Canvas) -> Cursor override
    {
        order->push_back(2);
        return {};
    }
    void paint_overlay(Canvas canvas) override
    {
        order->push_back(3);
        put(canvas, {}, U'!');
    }
    auto get_children() -> zzz::Generator<Widget&> override { co_yield child; }
    auto get_children() const -> zzz::Generator<Widget const&> override
    { co_yield child; }
};
}  // namespace
TEST(padding_and_paint_layers_preserve_child_content_between_background_and_overlay)
{
    auto order = std::vector<int>{};
    auto root = Padding{Layers{order}, std::array{1, 2, 1, 2}};
    auto app = Application{root};
    app.dispatch(esc::Resize{{12, 3}});
    app.paint();
    ASSERT((root.child.bounds() == Rect{{2, 1}, {8, 1}}));
    ASSERT((order == std::vector<int>{1, 2, 3}));
    ASSERT((app.loop().screen[Point{2, 1}].text == U"!"));
    ASSERT((app.loop().screen[Point{3, 1}].text == U"c"));
}

TEST(scrollview_measures_content_clips_translates_and_synchronizes_both_axes)
{
    auto view = ScrollView{PixelGrid{Area{20, 20}, RGB{0x123456}}};
    auto app = Application{view};
    app.dispatch(esc::Resize{{6, 3}});
    ASSERT((view.content_size() == Area{20, 10}));
    auto vertical = ScrollBar{};
    auto horizontal = ScrollBar{{.vertical = false}};
    auto ids = link(view, vertical);
    link(view, horizontal);
    ASSERT(view.set_scroll_offset({10, 5}));
    ASSERT((view.child.at() == Point{-10, -5}));
    ASSERT(vertical.position() == 5 && horizontal.position() == 10);
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].text == U"▄"));
    view.on_scroll.disconnect(ids.first);
    vertical.on_scroll.disconnect(ids.second);
    view.set_scroll_offset({0, 0});
    ASSERT(vertical.position() == 5);
    app.dispatch(esc::Resize{{30, 15}});
    ASSERT((view.scroll_offset() == Point{}));
}

TEST(text_notifications_allow_reentrant_edits_moves_and_owner_destruction)
{
    auto line = LineEdit{};
    auto moved = LineEdit{};
    line.on_change.connect([&](auto const& value) {
        if (value == "one") {
            moved = std::move(line);
            moved.set_text("two");
        }
    });
    auto seen = std::vector<std::string>{};
    line.on_change.connect([&](auto const& value) { seen.push_back(value); });
    line.set_text("one");
    ASSERT(moved.text() == "two" && seen.size() == 2);
    auto box = std::make_unique<TextBox>();
    box->on_change.connect([&](auto const&) { box.reset(); });
    box->set_text("destroy");
    ASSERT(!box);
}

TEST(invalid_container_measurements_and_text_batches_preserve_live_state)
{
    auto line = LineEdit{{.text = "safe"}};
    ASSERT_THROWS(line.edit([](TextEdit& edit) { edit = TextEdit{"wrong", true}; }),
                  std::invalid_argument);
    ASSERT(line.text() == "safe");
    auto padding = Padding{Widget{}};
    padding.padding[0] = -1;
    ASSERT_THROWS(padding.refresh(), std::invalid_argument);
    auto stack = Stack{Widget{}};
    ASSERT_THROWS(stack.select(1), std::out_of_range);
    ASSERT(stack.selected_index() == 0);
    ASSERT_THROWS((RadioGroup{{.items = {"one"}, .selected_index = 5}}),
                  std::out_of_range);
}
