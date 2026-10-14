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
    Connection{.signal = rg.on_select,
               .slot = [](std::optional<std::size_t> selection, auto& group, auto& tb) {
                   tb.set_text(selection ? group.items()[*selection] : "");
               }}(rg, tb);

    return Application{head}.run();
}