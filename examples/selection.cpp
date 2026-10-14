#include <ox/ox.hpp>
using namespace ox;
int main()
{
    auto root = Column{
        MenuBar{{{"File",
                  {{.text = "New", .action = [] {}},
                   {.text = "Export",
                    .children = {{.text = "Text", .action = [] {}},
                                 {.text = "CSV", .action = [] {}}}}}}}},
        Tabs{
            std::tuple{
                Column{
                    Label{
                        "List: Shift selects a range, Ctrl toggles. Enter activates."},
                    ListView{{.items = {{"Alpha"},
                                        {"Beta"},
                                        {"Disabled", false},
                                        {"Delta"},
                                        {"界界"},
                                        {"More"},
                                        {"Last"}},
                              .multiple = true}},
                    Select{{.items = {{"Red"}, {"Green"}, {"Blue"}}}},
                    ComboBox{{.items = {{"Apple"}, {"Apricot"}, {"Banana"}, {"Cherry"}},
                              .editor = {.placeholder =
                                             "Type to filter; F4 opens choices"}}}},
                Column{
                    RadioGroup{{.items = {"Small", "Medium", "Large", "Extra large"}}},
                    Button{{.label = {.text = "Open dialog"}}}}},
            std::vector<std::string>{"Selection", "Dialogs"}},
        Dialog{Column{Label{"A modal dialog. Tab stays here; Escape rejects."},
                      LineEdit{{.placeholder = "Name"}},
                      Row{Button{{.label = {.text = "OK"}}},
                          Button{{.label = {.text = "Cancel"}}}}},
               "Example"}};
    auto& [menus, tabs, dialog] = root.children;
    auto& page = std::get<1>(tabs.pages.children);
    auto& open = std::get<1>(page.children);
    auto& actions = std::get<2>(dialog.child.child.children);
    dialog.requested_size = Area{54, 9};
    Connection{.signal = open.on_activate,
               .slot = [](auto& dialog) { dialog.show(); }}(dialog);
    Connection{.signal = std::get<0>(actions.children).on_activate,
               .slot = [](auto& dialog) { dialog.accept(); }}(dialog);
    Connection{.signal = std::get<1>(actions.children).on_activate,
               .slot = [](auto& dialog) { dialog.reject(); }}(dialog);
    return Application{root}.run({.mouse_mode = MouseMode::Move});
}
