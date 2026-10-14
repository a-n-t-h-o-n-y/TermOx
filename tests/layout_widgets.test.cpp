#include <zzz/test.hpp>

#include <functional>
#include <list>
#include <ox/ox.hpp>

using namespace ox;

namespace {
struct Probe : Widget {
   public:
    using Widget::handle;
    std::vector<Area> old_sizes;
    explicit Probe(SizePolicy policy = SizePolicy::flex())
        : Widget{FocusPolicy::None, policy}
    {}
    void on_arrange(Rect previous) override { old_sizes.push_back(previous.size); }
};
}  // namespace

TEST(row_adapts_tuple_children_and_notifies_with_previous_sizes)
{
    auto row = Row{Probe{SizePolicy::fixed(2)}, Probe{}, Probe{SizePolicy::fixed(1)}};
    arrange(row, {row.at(), {10, 3}});
    ASSERT((get_child<0>(row).size() == Area{2, 3}));
    ASSERT((get_child<1>(row).at() == Point{2, 0}));
    ASSERT((get_child<1>(row).size() == Area{7, 3}));
    ASSERT((get_child<2>(row).at() == Point{9, 0}));
    ASSERT((get_child<1>(row).old_sizes == std::vector<Area>{{0, 0}}));
    arrange(row, {row.at(), {3, 1}});
    ASSERT((get_child<1>(row).size() == Area{0, 1}));
    ASSERT((get_child<1>(row).old_sizes.back() == Area{7, 3}));
}

TEST(column_adapts_dynamic_children_and_filters_inactive_policies)
{
    auto column = Column<std::vector<Probe>>{};
    column.children.emplace_back(SizePolicy::fixed(2));
    column.children.emplace_back(SizePolicy{-1, -2, -3});
    column.children.back().visible = false;
    column.children.emplace_back();
    arrange(column, {column.at(), {5, 8}});
    ASSERT((column.children[0].size() == Area{5, 2}));
    ASSERT((column.children[2].at() == Point{0, 2}));
    ASSERT((column.children[2].size() == Area{5, 6}));
    ASSERT(column.children[1].old_sizes.empty());
    column.children[0].visible = false;
    arrange(column, column.bounds());
    ASSERT((column.children[2].at() == Point{0, 0}));
    ASSERT((column.children[2].size() == Area{5, 8}));
}

TEST(nested_layouts_refresh_even_when_parent_size_is_unchanged)
{
    auto head = Column{Row{Probe{}, Probe{}}, Probe{SizePolicy::fixed(1)}};
    arrange(head, {head.at(), {9, 5}});
    auto& row = get_child<0>(head);
    ASSERT((get_child<0>(row).size() == Area{5, 4}));
    get_child<0>(row).size_policy = SizePolicy::fixed(2);
    arrange(head, head.bounds());
    ASSERT((get_child<0>(row).size() == Area{2, 4}));
    ASSERT((get_child<1>(row).at() == Point{2, 0}));
    ASSERT((get_child<1>(row).size() == Area{7, 4}));
}

TEST(layout_validation_precedes_changes_to_child_geometry)
{
    auto row = Row{Probe{}, Probe{SizePolicy::fixed(-1)}};
    ASSERT_THROWS((arrange(row, {row.at(), {8, 3}})), std::invalid_argument);
    ASSERT((get_child<0>(row).size() == Area{0, 0}));
    ASSERT(get_child<0>(row).old_sizes.empty());
    get_child<1>(row).size_policy = SizePolicy::flex();
    ASSERT_THROWS((arrange(row, {row.at(), {-1, 3}})), std::invalid_argument);
    ASSERT(get_child<0>(row).old_sizes.empty());
}

TEST(suspended_uses_explicit_area_and_clamps_then_centers)
{
    auto suspended = Suspended{Probe{}, Area{4, 2}};
    arrange(suspended, {suspended.at(), {9, 7}});
    ASSERT((suspended.child.at() == Point{2, 2}));
    ASSERT((suspended.child.size() == Area{4, 2}));
    arrange(suspended, {suspended.at(), {2, 1}});
    ASSERT((suspended.child.at() == Point{0, 0}));
    ASSERT((suspended.child.size() == Area{2, 1}));
    ASSERT((suspended.child.old_sizes.back() == Area{4, 2}));
    suspended.requested_size = {1, 0};
    arrange(suspended, suspended.bounds());
    ASSERT((suspended.child.size() == Area{1, 0}));
    ASSERT_THROWS((Suspended{Probe{}, Area{-1, 1}}), std::invalid_argument);
}

namespace {
struct LayoutHook : Widget {
    using Widget::handle;
    std::function<void()> action;
    int arrangements = 0;
    void on_arrange(Rect) override
    {
        ++arrangements;
        auto callback = action;  // A callback can relocate/destroy this hook.
        if (callback) { callback(); }
    }
};
}  // namespace

TEST(layout_refresh_handles_arrays_lists_empty_tuples_and_const_child_access)
{
    auto row = Row{std::array{Probe{SizePolicy::fixed(1)}, Probe{}}};
    arrange(row, {{}, {6, 2}});
    ASSERT((get_child<1>(std::as_const(row)).size() == Area{5, 2}));
    get_child<0>(row).size_policy = SizePolicy::fixed(3);
    row.refresh();
    ASSERT((get_child<1>(row).at() == Point{3, 0}));
    auto column = Column<std::list<Probe>>{};
    column.children.emplace_back();
    column.children.emplace_back();
    arrange(column, {{}, {2, 5}});
    ASSERT((column.children.front().size() == Area{2, 3}));
    auto empty = Row{std::tuple<>{}};
    arrange(empty, {{}, {2, 3}});
    int count = 0;
    for (auto const& child : std::as_const(empty).get_children()) {
        (void)child;
        ++count;
    }
    ASSERT(count == 0);
    auto border = Column{Probe{}, Probe{}} | Border{};
    arrange(border, {{}, {8, 6}});
    ASSERT((get_child<0>(std::as_const(border)).size() == Area{6, 2}));
}

TEST(layout_reentrant_refresh_replans_after_child_growth_and_policy_changes)
{
    auto row = Row<std::vector<LayoutHook>>{};
    row.children.emplace_back();
    row.children.emplace_back();
    bool changed = false;
    row.children[0].action = [&] {
        if (std::exchange(changed, true)) { return; }
        row.children.emplace_back();  // Relocates both existing children.
        row.children[0].size_policy = SizePolicy::fixed(2);
        row.refresh();
    };
    arrange(row, {{}, {10, 2}});
    ASSERT(row.children.size() == 3);
    ASSERT((row.children[0].size() == Area{2, 2}));
    ASSERT((row.children[1].bounds() == Rect{{2, 0}, {4, 2}}));
    ASSERT((row.children[2].bounds() == Rect{{6, 0}, {4, 2}}));
    ASSERT(row.children[1].arrangements == 1 && row.children[2].arrangements == 1);
    row.children[0].action = [&] { row.refresh(); };  // Redundant requests must settle.
    row.refresh();
    ASSERT(row.children[1].arrangements == 2);
}

TEST(layout_callbacks_can_remove_children_without_dereferencing_stale_targets)
{
    auto row = Row<std::vector<LayoutHook>>{};
    row.children.resize(3);
    bool changed = false;
    row.children[0].action = [&] {
        if (std::exchange(changed, true)) { return; }
        row.children.erase(row.children.begin() + 1);
        row.refresh();
    };
    arrange(row, {{}, {10, 1}});
    ASSERT(row.children.size() == 2);
    ASSERT((row.children[1].bounds() == Rect{{5, 0}, {5, 1}}));
    ASSERT(row.children[1].arrangements == 1);
}

TEST(layout_callbacks_can_move_or_destroy_the_parent)
{
    auto row = Row{LayoutHook{}, LayoutHook{}};
    auto destination = Row{LayoutHook{}, LayoutHook{}};
    get_child<0>(row).action = [&] { destination = std::move(row); };
    arrange(row, {{}, {8, 2}});
    ASSERT((get_child<1>(destination).bounds() == Rect{{4, 0}, {4, 2}}));
    get_child<0>(destination).action = {};
    destination.refresh();
    ASSERT(get_child<1>(destination).arrangements == 2);
    auto owner = std::make_unique<Row<std::vector<LayoutHook>>>();
    owner->children.emplace_back();
    owner->children[0].action = [&] { owner.reset(); };
    arrange(*owner, {{}, {4, 2}});
    ASSERT(!owner);
    auto sliced_row = Row{LayoutHook{}, LayoutHook{}};
    auto sliced = std::optional<Widget>{};
    get_child<0>(sliced_row).action = [&] {
        sliced.emplace(std::move(static_cast<Widget&>(sliced_row)));
    };
    arrange(sliced_row, {{}, {8, 2}});
    ASSERT(sliced && get_child<1>(sliced_row).arrangements == 0);
}

TEST(layout_throwing_callbacks_reset_arrangement_state_for_recovery)
{
    auto throwing = Row{LayoutHook{}, LayoutHook{}};
    get_child<0>(throwing).action = [] { throw std::runtime_error{"layout hook"}; };
    ASSERT_THROWS(arrange(throwing, {{}, {8, 2}}), std::runtime_error);
    get_child<0>(throwing).action = {};
    throwing.refresh();
    ASSERT((get_child<1>(throwing).bounds() == Rect{{4, 0}, {4, 2}}));
}

TEST(layout_refresh_schedules_application_paint_after_dynamic_child_edits)
{
    auto row = Row<std::vector<Label>>{};
    row.children.emplace_back("a");
    arrange(row, {{}, {4, 1}});
    auto app = Application{row};
    app.dispatch(esc::Resize{row.size()});
    app.paint();
    row.children.emplace_back("b");
    row.refresh();
    ASSERT(app.loop().needs_repaint());
    ASSERT(app.paint());
    ASSERT((app.loop().screen[Point{0, 0}].text == U"a"));
    ASSERT((app.loop().screen[Point{2, 0}].text == U"b"));
    row.children.front().visible = false;
    row.refresh();
    app.paint();
    ASSERT((app.loop().screen[Point{1, 0}].text == U"b"));
}

TEST(layout_refresh_settles_after_a_child_is_moved_out_without_replacing_its_slot)
{
    auto row = Row{Probe{}, Probe{}};
    arrange(row, {{}, {8, 2}});
    auto extracted = std::move(get_child<0>(row));
    row.refresh();
    ASSERT((extracted.bounds() == Rect{{0, 0}, {4, 2}}));
    ASSERT(get_child<1>(row).old_sizes.size() == 2);
}
