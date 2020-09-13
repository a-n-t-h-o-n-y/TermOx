#ifndef CPPURSES_DEMOS_FOCUS_FOCUS_DEMO_HPP
#define CPPURSES_DEMOS_FOCUS_FOCUS_DEMO_HPP
#include <memory>

#include <cppurses/painter/color.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/label.hpp>

namespace demos::focus {

auto focus_box(cppurses::Focus_policy policy)
    -> std::unique_ptr<cppurses::Widget>
{
    using namespace cppurses::pipe;

    /// Focus_policy to string
    auto to_string = [](cppurses::Focus_policy p) -> wchar_t const* {
        switch (p) {
            using namespace cppurses;
            case Focus_policy::None: return L"None";
            case Focus_policy::Tab: return L"Tab";
            case Focus_policy::Click: return L"Click";
            case Focus_policy::Strong: return L"Strong";
            case Focus_policy::Direct: return L"Direct";
        }
        return L"";
    };

    /// Remove tab focus from \p p.
    auto const narrow = [](cppurses::Focus_policy p) {
        switch (p) {
            using namespace cppurses;
            case Focus_policy::None:
            case Focus_policy::Tab: return Focus_policy::None;
            case Focus_policy::Click:
            case Focus_policy::Strong: return Focus_policy::Click;
            case Focus_policy::Direct: return Focus_policy::Direct;
        }
        return cppurses::Focus_policy::None;
    };

    // clang-format off
    auto box_ptr =
        cppurses::layout::vertical
        (
            cppurses::label<cppurses::layout::Horizontal<>>({to_string(policy)})
                | name("l")
                | align_center()
                | fixed_height(1)
                | cppurses::pipe::focus(narrow(policy)),
            cppurses::widget()
                | name("w")
                | cppurses::pipe::focus(policy)
        ) | bordered();

    box_ptr | children() | find("l")
            | on_focus_in([w = box_ptr->find("w")]{ cppurses::System::set_focus(*w); });

    box_ptr | children() | find("w")
            | on_focus_in( [&w = *box_ptr]{ w | walls(foreground(cppurses::Color::Red)); })
            | on_focus_out([&w = *box_ptr]{ w | walls(foreground(cppurses::Color::White)); });
    // clang-format on

    return box_ptr;
}

/// Build a focus app demo and return the owning pointer to it.
auto build_demo() -> std::unique_ptr<cppurses::Widget>
{
    using namespace cppurses;
    using namespace cppurses::pipe;

    // clang-format off
    return
    layout::horizontal(
        layout::vertical(
            focus_box(Focus_policy::Tab) | height_stretch(3),
            layout::horizontal(
                focus_box(Focus_policy::Strong),
                focus_box(Focus_policy::Tab)
            )
        ),
        layout::vertical(
            focus_box(Focus_policy::Strong),
            focus_box(Focus_policy::None)
        ),
        layout::vertical(
            focus_box(Focus_policy::Click),
            layout::horizontal(
                focus_box(Focus_policy::Strong),
                layout::vertical(
                    focus_box(Focus_policy::None),
                    focus_box(Focus_policy::Tab)
                ),
                focus_box(Focus_policy::Tab)
            ) | height_stretch(2),
            focus_box(Focus_policy::Strong)
        )
    );
    // clang-format on
}

}  // namespace demos::focus
#endif  // CPPURSES_DEMOS_FOCUS_FOCUS_DEMO_HPP
