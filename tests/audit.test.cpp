#include <zzz/test.hpp>

#include <functional>
#include <limits>
#include <list>
#include <memory>

#include <ox/ox.hpp>

using namespace ox;

TEST(signal_connections_borrow_const_and_movable_targets_and_disconnect_explicitly)
{
    auto signal = sl::Signal<void(int)>{};
    auto target = Label{{.text = "before"}};
    auto const fixed = Label{{.text = "constant"}};
    auto calls = 0;
    auto id = connect(
        signal,
        [&](int value, Label& label, Label const& other) {
            label.text = std::to_string(value) + other.text;
            ++calls;
        },
        target, fixed);
    signal(1);
    ASSERT(target.text == "1constant" && calls == 1);
    auto moved = Label{std::move(target)};
    signal(2);
    ASSERT(moved.text == "2constant" && calls == 2);
    auto moved_signal = std::move(signal);
    auto slot = moved_signal.disconnect(id);
    moved_signal(3);
    ASSERT(calls == 2);
    slot(4);
    ASSERT(moved.text == "4constant" && calls == 3);
}

TEST(
    signal_callbacks_are_suppressed_after_destruction_or_slicing_without_owning_targets)
{
    auto signal = sl::Signal<void()>{};
    auto target = std::make_unique<Label>();
    auto calls = 0;
    auto capture = std::make_shared<int>(1);
    auto weak = std::weak_ptr{capture};
    auto id = connect(signal, [capture, &calls](Label&) { ++calls; }, *target);
    capture.reset();
    signal();
    ASSERT(calls == 1);
    target.reset();
    signal();
    ASSERT(calls == 1 && !weak.expired());
    signal.disconnect(id);
    ASSERT(weak.expired());
    auto derived = Label{};
    connect(signal, [&](Label&) { ++calls; }, derived);
    auto sliced = Widget{std::move(derived)};
    signal();
    ASSERT(calls == 1);
    auto plain = 0;
    auto plain_id = connect(signal, [&] { ++plain; });
    signal();
    ASSERT(plain == 1);
    signal.disconnect(plain_id);
    signal();
    ASSERT(plain == 1);
    auto victim = std::make_unique<Widget>();
    connect(
        signal,
        [&](Widget&) {
            victim.reset();
            signal();  // Reentrant delivery suppresses the now-expired target.
            ++plain;
        },
        *victim);
    signal();
    signal();
    ASSERT(plain == 2);
}

TEST(signal_connections_reject_empty_tracked_callbacks_before_subscribing)
{
    auto target = Widget{};
    auto signal = sl::Signal<void()>{};
    auto callback = std::function<void(Widget&)>{};
    ASSERT_THROWS(connect(signal, callback, target), std::invalid_argument);
    ASSERT_THROWS(tracked(callback, target), std::invalid_argument);
    auto empty = std::function<void()>{};
    ASSERT_THROWS(connect(signal, empty), std::invalid_argument);
    signal();
}

TEST(glyph_ranges_apply_style_patches_identically_for_vectors_lists_and_spans)
{
    auto glyphs = U"é界"_gs;
    auto list = std::list<Glyph>{glyphs.begin(), glyphs.end()};
    auto one = Frame{{5, 1}};
    auto two = Frame{{5, 1}};
    auto three = Frame{{5, 1}};
    auto patch = StylePatch{};
    patch.foreground = colors::red;
    put(Canvas{one}, {}, glyphs, patch);
    put(Canvas{two}, {}, list, patch);
    put(Canvas{three}, {}, std::span<Glyph const>{glyphs}, patch);
    ASSERT(std::ranges::equal(one.row(0), two.row(0)));
    ASSERT(std::ranges::equal(one.row(0), three.row(0)));
    ASSERT((one[Point{0, 0}].text == U"é"));
    ASSERT((one[Point{1, 0}].style.foreground == Color{colors::red}));
}

TEST(fill_outside_matches_a_rectangle_reference_across_clips_and_extreme_bounds)
{
    for (auto content : std::array{
             Rect{{2, 1}, {3, 2}}, Rect{{-1, -1}, {3, 3}}, Rect{{10, 10}, {1, 1}},
             Rect{{2, 2}, {0, 3}}, Rect{{-100, -100}, {200, 200}},
             Rect{{std::numeric_limits<int>::max(), 0}, {1, 1}}}) {
        auto frame = Frame{{9, 7}};
        fill(Canvas{frame}, U'x');
        auto view = Canvas{frame}.subcanvas({1, 1}, {7, 5}).subcanvas({-1, 0}, {8, 5});
        fill_outside(view, content, U'.' | fg(colors::red));
        for (int y = 0; y < 7; ++y) {
            for (int x = 0; x < 9; ++x) {
                auto const local = Point{x, y - 1};
                auto const painted = view.contains(local) && !contains(content, local);
                ASSERT((frame[Point{x, y}].text == (painted ? U"." : U"x")));
            }
        }
    }
    auto frame = Frame{{1, 1}};
    auto const maximum = std::numeric_limits<int>::max();
    // An invisible view needs no overflowing global margin coordinates.
    fill_outside(Canvas{frame, {maximum, maximum}, {maximum, maximum}},
                 {{1, 1}, {1, 1}});
    ASSERT((frame[Point{0, 0}].text == U" "));
}

TEST(suspended_refresh_updates_explicit_geometry_and_paints_only_margins)
{
    auto panel = Suspended{Widget{}, Area{3, 1}, U'.' | fg(colors::red)};
    arrange(panel, {{}, {7, 5}});
    auto frame = Frame{{7, 5}};
    fill(Canvas{frame}, U'x');
    panel.paint(Canvas{frame});
    ASSERT((frame[Point{2, 2}].text == U"x"));
    ASSERT((frame[Point{1, 2}].text == U"."));
    panel.requested_size = {5, 3};
    panel.refresh();
    ASSERT((panel.child.bounds() == Rect{{1, 1}, {5, 3}}));
    panel.requested_size = {0, 0};
    panel.refresh();
    panel.paint(Canvas{frame});
    for (auto cell : frame.row(2)) {
        ASSERT(cell.text == U".");
    }
}

TEST(margin_drawing_and_suspended_refresh_validate_before_mutation)
{
    auto frame = Frame{{4, 2}};
    fill(Canvas{frame}, U'x');
    ASSERT_THROWS(fill_outside(Canvas{frame}, {{}, {-1, 1}}), std::invalid_argument);
    ASSERT_THROWS(fill_outside(Canvas{frame}, {{}, {4, 2}}, U'\n' | Style{}),
                  std::invalid_argument);
    ASSERT((frame[Point{0, 0}].text == U"x"));
    auto panel = Suspended{Widget{}, Area{2, 1}};
    arrange(panel, {{}, {4, 2}});
    auto const bounds = panel.child.bounds();
    panel.requested_size = {-1, 2};
    ASSERT_THROWS(panel.refresh(), std::invalid_argument);
    ASSERT(panel.child.bounds() == bounds);
}

TEST(alignment_uses_columns_and_rejects_invalid_extents_and_values)
{
    ASSERT(align_offset(Align::Left, 10, 4) == 0);
    ASSERT(align_offset(Align::Center, 10, 4) == 3);
    ASSERT(align_offset(Align::Right, 10, 4) == 6);
    ASSERT(align_offset(Align::Right, 4, 10) == -6);
    ASSERT_THROWS(align_offset(Align::Left, -1, 2), std::invalid_argument);
    ASSERT_THROWS(align_offset(Align::Center, 2, -1), std::invalid_argument);
    ASSERT_THROWS(align_offset((Align)99, 2, 1), std::invalid_argument);
}

TEST(margin_fill_preserves_complete_wide_content_and_tiles_wide_margins)
{
    auto frame = Frame{{6, 1}};
    frame.write({2, 0}, U"🙂", {.foreground = colors::blue});
    fill_outside(Canvas{frame}, {{2, 0}, {2, 1}}, U'界' | fg(colors::red));
    ASSERT((frame[Point{0, 0}].text == U"界" && frame[Point{0, 0}].columns == 2));
    ASSERT((frame[Point{2, 0}].text == U"🙂"));
    ASSERT((frame[Point{2, 0}].style.foreground == Color{colors::blue}));
    ASSERT((frame[Point{4, 0}].text == U"界" && frame[Point{5, 0}].columns == 0));
}
