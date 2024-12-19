#include <ox/ox.hpp>
using namespace ox;

int main()
{
    auto head = TextBox{} | FocusPolicy::Strong | Border::round("TextBox");
    return Application{head}.run();
}