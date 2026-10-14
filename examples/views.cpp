#include <ox/ox.hpp>
using namespace ox;
int main()
{
    auto root = Column{
        Label{"Tab: focus | Ctrl-Tab: pages | Ctrl-Enter: form | Ctrl-C: exit"},
        Tabs{std::tuple{
                 Column{Slider{{.range = {0, 100, 5}, .value = 30}},
                        NumericEdit{{.range = {0, 100, 5}, .value = 30,
                                     .format = "{:.2f}", .suffix = " kg"}},
                        ProgressBar{{.value = 30}},
                        Row{Spinner{}, Switch{{.label = {.text = "Enabled"}}}},
                        LogView{{.max_lines = 200}}},
                 SplitPane{
                     TreeView{
                         {.nodes =
                              {{"Project",
                                true,
                                true,
                                {{"Source", true, true, {{"main.cpp"}, {"widget.cpp"}}},
                                 {"Tests", true, false, {{"unit.cpp"}}}}},
                               {"README.md"}}}},
                     LogView{{.max_lines = 100}}},
                 Form{std::tuple{LineEdit{{.placeholder = "Required"}},
                                 IntegerEdit{{.range = {0, 120, 1}, .value = 18}},
                                 Switch{{.label = {.text = "Notifications"}}}},
                      std::vector<FormField>{
                          {"Name", "",
                           [](Widget const& w) -> std::optional<std::string> {
                               return static_cast<LineEdit const&>(w).text().empty()
                                          ? std::optional<std::string>{"Enter a name"}
                                          : std::nullopt;
                           }},
                          {"Age", "0–120"},
                          {"Settings"}}},
                 FilePicker{},
                 CommandPalette{{{"Open file", "file", true, [] {}},
                                 {"Save file", "write", true, [] {}},
                                 {"Toggle theme", "appearance", true, [] {}},
                                 {"Unavailable command", "", false}}}},
             std::vector<std::string>{"Values", "Tree / Log", "Form", "Files",
                                      "Commands"}}};
    auto& pages = std::get<1>(root.children).pages.children;
    auto& values = std::get<0>(pages).children;
    auto& slider = std::get<0>(values);
    auto& number = std::get<1>(values);
    auto& progress = std::get<2>(values);
    auto& log = std::get<4>(values);
    Connection{.signal = slider.on_change,
               .slot = [](double v, NumericEdit& n) { n.set_value(v); }}(number);
    Connection{.signal = number.on_change,
               .slot = [](double v, Slider& s, ProgressBar& p, LogView& l) {
                   s.set_value(v);
                   p.set_value(v);
                   l.append("Value: " + std::to_string(v));
               }}(slider, progress, log);
    log.append("Drag the slider or edit the number. Enter commits; Up/Down steps.");
    auto& split = std::get<1>(pages);
    Connection{.signal = split.first.on_activate,
               .slot = [](auto const& path, LogView& log) {
                   std::string text = "Activated:";
                   for (auto i : path)
                       text += " /" + std::to_string(i);
                   log.append(text);
               }}(split.second);
    split.second.append("Tree: Right expands, Left collapses. Drag the divider.");
    return Application{root}.run({.mouse_mode = MouseMode::Move});
}
