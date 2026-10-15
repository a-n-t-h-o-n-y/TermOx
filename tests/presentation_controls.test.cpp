#include <ox/ox.hpp>
#include <zzz/test.hpp>
using namespace ox;

TEST(list_presentation_orders_stripes_hover_selection_and_inactive_styles)
{
    auto list = ListView{{.items = {{"first"}, {"second"}, {"disabled", false}},
                          .inactive_selected_style = {.background = {},
                                                      .foreground = colors::blue,
                                                      .enabled = {},
                                                      .disabled = {}},
                          .alternate_style = {.background = colors::green,
                                              .foreground = {},
                                              .enabled = {},
                                              .disabled = {}},
                          .hover_style = {.background = colors::red,
                                          .foreground = {},
                                          .enabled = {},
                                          .disabled = {}},
                          .indicator_style = {.background = {},
                                              .foreground = colors::cyan,
                                              .enabled = {},
                                              .disabled = {}},
                          .striped = true,
                          .hover_rows = true}};
    auto app = Application{list};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{14, 3}});
    list.handle(MouseMove{.at = {3, 1}, .held_button = {}});
    app.paint();
    ASSERT(list.hovered_row() == 1);
    ASSERT((app.loop().screen[Point{3, 1}].style.background == Color{colors::red}));
    list.select(1);
    app.paint();
    ASSERT((app.loop().screen[Point{3, 1}].style.background ==
            app.theme().palette.selection));
    ASSERT((app.loop().screen[Point{0, 1}].style.foreground == Color{colors::cyan}));
    app.clear_focus();
    app.paint();
    ASSERT((app.loop().screen[Point{3, 1}].style.foreground == Color{colors::blue}));
    list.mouse_leave();
    list.select({});
    app.paint();
    ASSERT(!list.hovered_row());
    ASSERT((app.loop().screen[Point{3, 1}].style.background == Color{colors::green}));
    list.handle(MouseMove{.at = {3, 2}, .held_button = {}});
    app.request_repaint();
    app.paint();
    ASSERT((app.loop().screen[Point{3, 2}].style.background != Color{colors::red}));
}

TEST(list_hover_tracks_viewport_scrolling_and_empty_states)
{
    auto list = ListView{{.items = {{"zero"}, {"one"}, {"two"}, {"three"}},
                          .hover_rows = true,
                          .show_indicator = false,
                          .empty_text = "No matches"}};
    auto app = Application{list};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{12, 2}});
    ASSERT(list.content_size().width == 5);
    list.handle(MouseMove{.at = {1, 1}, .held_button = {}});
    list.set_scroll_offset({0, 2});
    ASSERT(list.hovered_row() == 3);
    list.set_items({});
    ASSERT(!list.hovered_row());
    ASSERT((list.content_size() == Area{}));
    ASSERT((list.measure({12, 2}) == Area{10, 1}));
    list.empty_style.foreground = colors::red;
    list.refresh();
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].text == U"N"));
    ASSERT((app.loop().screen[Point{0, 0}].style.foreground == Color{colors::red}));
    for (auto size : {Area{0, 0}, Area{1, 1}}) {
        app.dispatch(esc::Resize{size});
        app.paint();
    }
}

TEST(tree_node_styles_and_guides_align_without_selection_markers)
{
    auto leaf = TreeNode{"leaf"};
    leaf.style.foreground = colors::green;
    auto tree = TreeView{{.nodes = {{"root", true, true, {leaf}}},
                          .list = {.show_indicator = false},
                          .guide_style = {.background = {},
                                          .foreground = colors::red,
                                          .enabled = {},
                                          .disabled = {}}}};
    auto app = Application{tree};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{14, 3}});
    app.paint();
    ASSERT((app.loop().screen[Point{0, 1}].text == U"└"));
    ASSERT((app.loop().screen[Point{0, 1}].style.foreground == Color{colors::red}));
    ASSERT((app.loop().screen[Point{4, 1}].style.foreground == Color{colors::green}));
    tree.show_guides = false;
    tree.refresh();
    app.paint();
    ASSERT((app.loop().screen[Point{0, 1}].text == U" "));
    tree.show_guides = true;
    tree.refresh();
    app.set_theme(Theme::terminal());
    app.paint();
    ASSERT((app.loop().screen[Point{0, 1}].text == U"`"));
    ASSERT((app.loop().screen[Point{0, 0}].text == U"v"));
    tree.select({0, 0});
    ASSERT((tree.current_path() == std::vector<std::size_t>{0, 0}));
}

TEST(choice_indicators_have_independent_colors_and_preserve_explicit_glyphs)
{
    auto box = CheckBox{{.checked = true,
                         .label = {.text = "Enabled"},
                         .checked_style = {.background = {},
                                           .foreground = colors::green,
                                           .enabled = {},
                                           .disabled = {}}}};
    auto app = Application{box};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{14, 1}});
    app.paint();
    ASSERT((app.loop().screen[Point{1, 0}].style.foreground == Color{colors::green}));
    ASSERT((app.loop().screen[Point{4, 0}].style.foreground ==
            app.theme().palette.foreground));
    box.checked_style = {};
    for (auto& glyph : box.checked_glyphs)
        glyph.style.foreground = colors::blue;
    box.refresh();
    app.paint();
    ASSERT((app.loop().screen[Point{1, 0}].style.foreground == Color{colors::blue}));
    box.enabled = false;
    box.refresh();
    app.paint();
    ASSERT((app.loop().screen[Point{1, 0}].style.traits.contains(Trait::Dim)));
    auto toggle = Switch{{.checked = true,
                          .checked_style = {.background = {},
                                            .foreground = colors::cyan,
                                            .enabled = {},
                                            .disabled = {}}}};
    arrange(toggle, {{}, {12, 1}});
    auto frame = Frame{toggle.size()};
    toggle.paint(Canvas{frame});
    ASSERT((frame[Point{0, 0}].style.foreground == Color{colors::cyan}));
}

TEST(radio_indicator_roles_survive_moves_and_leave_labels_neutral)
{
    auto radio = RadioGroup{{.items = {"One", "Two"},
                             .selected_index = 1,
                             .indicator_style = {.background = colors::black,
                                                 .foreground = {},
                                                 .enabled = {},
                                                 .disabled = {}},
                             .selected_style = {.background = {},
                                                .foreground = colors::green,
                                                .enabled = {},
                                                .disabled = {}},
                             .unselected_style = {.background = {},
                                                  .foreground = colors::red,
                                                  .enabled = {},
                                                  .disabled = {}}}};
    auto moved = std::move(radio);
    auto assigned = RadioGroup{};
    assigned = std::move(moved);
    auto app = Application{assigned};
    app.set_theme(Theme::light());
    app.dispatch(esc::Resize{{12, 2}});
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].style.foreground == Color{colors::red}));
    ASSERT((app.loop().screen[Point{0, 1}].style.foreground == Color{colors::green}));
    ASSERT((app.loop().screen[Point{4, 1}].style.foreground ==
            app.theme().palette.foreground));
    assigned.set_item_enabled(1, false);
    app.paint();
    ASSERT((app.loop().screen[Point{0, 1}].style.traits.contains(Trait::Dim)));
}

TEST(button_state_patches_survive_moves_and_pressed_style_wins_over_hover)
{
    auto button = Button{{.label = {.text = "Run"},
                          .pressed_style = {.background = {},
                                            .foreground = colors::red,
                                            .enabled = {},
                                            .disabled = {}},
                          .hover_style = {.background = {},
                                          .foreground = colors::green,
                                          .enabled = {},
                                          .disabled = {}},
                          .blurred_style = {.background = {},
                                            .foreground = colors::blue,
                                            .enabled = {},
                                            .disabled = {}},
                          .show_focus_indicator = false}};
    auto moved = std::move(button);
    auto assigned = Button{};
    assigned = std::move(moved);
    auto app = Application{assigned};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{12, 1}});
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].text == U" "));
    assigned.mouse_enter();
    app.request_repaint();
    app.paint();
    ASSERT((app.loop().screen[Point{4, 0}].style.foreground == Color{colors::green}));
    app.dispatch(MousePress{.at = {4, 0}, .button = MouseButton::Left});
    app.paint();
    ASSERT((app.loop().screen[Point{4, 0}].style.foreground == Color{colors::red}));
    assigned.pointer_cancel();
    assigned.mouse_leave();
    app.clear_focus();
    app.paint();
    ASSERT((app.loop().screen[Point{4, 0}].style.foreground == Color{colors::blue}));
}

TEST(slider_channels_work_in_both_orientations_with_optional_fill_and_thumb)
{
    for (bool vertical : {false, true}) {
        auto slider = Slider{{.value = 50,
                              .vertical = vertical,
                              .track_style = {.background = {},
                                              .foreground = colors::red,
                                              .enabled = {},
                                              .disabled = {}},
                              .fill_style = {.background = {},
                                             .foreground = colors::green,
                                             .enabled = {},
                                             .disabled = {}},
                              .thumb_style = {.background = {},
                                              .foreground = colors::blue,
                                              .enabled = {},
                                              .disabled = {}}}};
        auto moved = std::move(slider);
        auto assigned = Slider{};
        assigned = std::move(moved);
        auto app = Application{assigned};
        app.set_theme(Theme::terminal());
        app.dispatch(esc::Resize{vertical ? Area{1, 5} : Area{5, 1}});
        app.paint();
        auto point = [&](int index) {
            return vertical ? Point{0, 4 - index} : Point{index, 0};
        };
        ASSERT(app.loop().screen[point(0)].style.foreground == Color{colors::green});
        ASSERT(app.loop().screen[point(2)].style.foreground == Color{colors::blue});
        ASSERT(app.loop().screen[point(4)].style.foreground == Color{colors::red});
        assigned.show_fill = assigned.show_thumb = false;
        assigned.refresh();
        app.paint();
        ASSERT(app.loop().screen[point(0)].style.foreground == Color{colors::red});
        ASSERT(app.loop().screen[point(2)].text != U"o");
        app.dispatch(esc::Resize{{0, 0}});
        app.paint();
    }
}

TEST(progress_labels_tracks_and_fills_have_independent_styles)
{
    auto bar = ProgressBar{{.value = 50,
                            .track_style = {.background = {},
                                            .foreground = colors::red,
                                            .enabled = {},
                                            .disabled = {}},
                            .fill_style = {.background = {},
                                           .foreground = colors::green,
                                           .enabled = {},
                                           .disabled = {}},
                            .label_style = {.background = {},
                                            .foreground = colors::blue,
                                            .enabled = {},
                                            .disabled = {}}}};
    auto app = Application{bar};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{12, 1}});
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].style.foreground == Color{colors::green}));
    ASSERT((app.loop().screen[Point{11, 0}].style.foreground == Color{colors::red}));
    ASSERT((app.loop().screen[Point{4, 0}].style.foreground == Color{colors::blue}));
    ASSERT((app.loop().screen[Point{4, 0}].text == U"5"));
    bar.show_percentage = false;
    bar.refresh();
    app.paint();
    ASSERT((app.loop().screen[Point{4, 0}].style.foreground == Color{colors::green}));
}

TEST(scrollbar_and_spinner_roles_survive_changes_and_moves)
{
    auto bar = ScrollBar{{.content_length = 10,
                          .viewport_length = 2,
                          .vertical = false,
                          .track_style = {.background = {},
                                          .foreground = colors::red,
                                          .enabled = {},
                                          .disabled = {}},
                          .thumb_style = {.background = {},
                                          .foreground = colors::green,
                                          .enabled = {},
                                          .disabled = {}}}};
    auto moved = std::move(bar);
    auto assigned = ScrollBar{};
    assigned = std::move(moved);
    auto app = Application{assigned};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{10, 1}});
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].style.foreground == Color{colors::green}));
    ASSERT((app.loop().screen[Point{9, 0}].style.foreground == Color{colors::red}));
    auto spinner = Spinner{{.frames = {"x", "y"},
                            .running_style = {.background = {},
                                              .foreground = colors::green,
                                              .enabled = {},
                                              .disabled = {}},
                            .stopped_style = {.background = {},
                                              .foreground = colors::red,
                                              .enabled = {},
                                              .disabled = {}}}};
    arrange(spinner, {{}, {2, 1}});
    auto frame = Frame{spinner.size()};
    spinner.paint(Canvas{frame});
    ASSERT((frame[Point{0, 0}].style.foreground == Color{colors::green}));
    spinner.set_running(false);
    spinner.paint(Canvas{frame});
    ASSERT((frame[Point{0, 0}].style.foreground == Color{colors::red}));
}

TEST(tab_separator_visibility_updates_page_geometry_and_style_roles)
{
    auto tabs = Tabs{std::tuple{Label{{.text = "page"}}}, {"One"}};
    tabs.unselected_style.foreground = colors::green;
    tabs.indicator_style.foreground = colors::red;
    tabs.track_style.foreground = colors::blue;
    auto app = Application{tabs};
    auto theme = Theme::dark();
    theme.motion.enabled = false;
    app.set_theme(theme);
    app.dispatch(esc::Resize{{16, 4}});
    app.paint();
    ASSERT(tabs.pages.at().y == 2);
    ASSERT((app.loop().screen[Point{0, 1}].style.foreground == Color{colors::red}));
    ASSERT((app.loop().screen[Point{15, 1}].style.foreground == Color{colors::blue}));
    tabs.show_separator = false;
    tabs.select({});
    tabs.refresh();
    app.step();
    app.paint();
    ASSERT(tabs.pages.at().y == 1);
    ASSERT((app.loop().screen[Point{1, 0}].style.foreground == Color{colors::green}));
}

TEST(glyph_defaults_fill_unspecified_colors_before_explicit_patches)
{
    auto glyphs = U"界é"_gs;
    glyphs.front().style.foreground = colors::green;
    auto frame = Frame{{5, 1}};
    auto defaults = Style{
        .background = colors::black, .foreground = colors::red, .traits = Trait::Bold};
    auto patch = StylePatch{.background = {},
                            .foreground = {},
                            .enabled = Trait::Underline,
                            .disabled = Trait::Bold};
    auto view = glyphs | std::views::transform([](Glyph glyph) { return glyph; });
    put(Canvas{frame}, {}, view, patch, defaults);
    ASSERT((frame[Point{0, 0}].style.foreground == Color{colors::green}));
    ASSERT((frame[Point{2, 0}].style.foreground == Color{colors::red}));
    ASSERT((frame[Point{2, 0}].text == U"é"));
    ASSERT((frame[Point{2, 0}].style.background == Color{colors::black}));
    ASSERT((frame[Point{2, 0}].style.traits.contains(Trait::Underline)));
    ASSERT((!frame[Point{2, 0}].style.traits.contains(Trait::Bold)));
}
