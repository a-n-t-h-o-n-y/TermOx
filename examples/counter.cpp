#include <ox/ox.hpp>

using namespace ox;

int main()
{
    auto root = Column{
        IntegerLabel{{.value = 0}},
        Button{{.label = {.text = "Increment"}}},
    };
    auto& [count, button] = root.children;
    Connection{
        .signal = button.on_activate,
        .slot = [](IntegerLabel& count) { count.set_value(count.value + 1); },
    }(count);

    return Application{root}.run();
}
