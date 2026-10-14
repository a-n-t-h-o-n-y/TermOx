#include <ox/ox.hpp>

using namespace ox;

int main()
{
    auto head = Row{
        RadioGroup{{
            .items = {"one", "two", "three"},
            .focus_policy = FocusPolicy::Strong,
        }},
        TextBox{{.focus_policy = FocusPolicy::Strong}},
    };
    auto& [rg, tb] = head.children;
    connect(
        rg.on_select,
        [](std::string const& selection, auto& tb) { tb.set_text(selection); }, tb);

    return Application{head}.run();
}