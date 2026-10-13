#include <zzz/test.hpp>

#include <ox/layout.hpp>

using namespace ox;

namespace {
struct Probe : Widget {
    std::vector<Area> old_sizes;
    explicit Probe(SizePolicy policy = SizePolicy::flex())
        : Widget{FocusPolicy::None, policy}
    {}
    void resize(Area old_size) override { old_sizes.push_back(old_size); }
};
}  // namespace

TEST(row_adapts_tuple_children_and_notifies_with_previous_sizes)
{
    auto row = Row{Probe{SizePolicy::fixed(2)}, Probe{}, Probe{SizePolicy::fixed(1)}};
    row.size = {10, 3};
    row.resize({});
    ASSERT((get_child<0>(row).size == Area{2, 3}));
    ASSERT((get_child<1>(row).at == Point{2, 0}));
    ASSERT((get_child<1>(row).size == Area{7, 3}));
    ASSERT((get_child<2>(row).at == Point{9, 0}));
    ASSERT((get_child<1>(row).old_sizes == std::vector<Area>{{0, 0}}));
    row.size = {3, 1};
    row.resize({10, 3});
    ASSERT((get_child<1>(row).size == Area{0, 1}));
    ASSERT((get_child<1>(row).old_sizes.back() == Area{7, 3}));
}

TEST(column_adapts_dynamic_children_and_filters_inactive_policies)
{
    auto column = Column<std::vector<Probe>>{};
    column.children.emplace_back(SizePolicy::fixed(2));
    column.children.emplace_back(SizePolicy{-1, -2, -3});
    column.children.back().active = false;
    column.children.emplace_back();
    column.size = {5, 8};
    column.resize({});
    ASSERT((column.children[0].size == Area{5, 2}));
    ASSERT((column.children[2].at == Point{0, 2}));
    ASSERT((column.children[2].size == Area{5, 6}));
    ASSERT(column.children[1].old_sizes.empty());
    column.children[0].active = false;
    column.resize(column.size);
    ASSERT((column.children[2].at == Point{0, 0}));
    ASSERT((column.children[2].size == Area{5, 8}));
}

TEST(nested_layouts_refresh_even_when_parent_size_is_unchanged)
{
    auto head = Column{Row{Probe{}, Probe{}}, Probe{SizePolicy::fixed(1)}};
    head.size = {9, 5};
    head.resize({});
    auto& row = get_child<0>(head);
    ASSERT((get_child<0>(row).size == Area{5, 4}));
    get_child<0>(row).size_policy = SizePolicy::fixed(2);
    head.resize(head.size);
    ASSERT((get_child<0>(row).size == Area{2, 4}));
    ASSERT((get_child<1>(row).at == Point{2, 0}));
    ASSERT((get_child<1>(row).size == Area{7, 4}));
}

TEST(layout_validation_precedes_changes_to_child_geometry)
{
    auto row = Row{Probe{}, Probe{SizePolicy::fixed(-1)}};
    row.size = {8, 3};
    ASSERT_THROWS(row.resize({}), std::invalid_argument);
    ASSERT((get_child<0>(row).size == Area{0, 0}));
    ASSERT(get_child<0>(row).old_sizes.empty());
    get_child<1>(row).size_policy = SizePolicy::flex();
    row.size = {-1, 3};
    ASSERT_THROWS(row.resize({}), std::invalid_argument);
    ASSERT(get_child<0>(row).old_sizes.empty());
}

TEST(suspended_uses_explicit_area_and_clamps_then_centers)
{
    auto suspended = Suspended{Probe{}, Area{4, 2}};
    suspended.size = {9, 7};
    suspended.resize({});
    ASSERT((suspended.child.at == Point{2, 2}));
    ASSERT((suspended.child.size == Area{4, 2}));
    suspended.size = {2, 1};
    suspended.resize({9, 7});
    ASSERT((suspended.child.at == Point{0, 0}));
    ASSERT((suspended.child.size == Area{2, 1}));
    ASSERT((suspended.child.old_sizes.back() == Area{4, 2}));
    suspended.requested_size = {1, 0};
    suspended.resize(suspended.size);
    ASSERT((suspended.child.size == Area{1, 0}));
    ASSERT_THROWS((Suspended{Probe{}, Area{-1, 1}}), std::invalid_argument);
}
