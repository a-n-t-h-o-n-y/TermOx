#ifndef TERMOX_DEMOS_FOCUS_FOCUS_DEMO_HPP
#define TERMOX_DEMOS_FOCUS_FOCUS_DEMO_HPP
#include <memory>

#include <termox/painter/color.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/label.hpp>

namespace demos::focus {

inline auto focus_box(ox::Focus_policy policy) -> std::unique_ptr<ox::Widget>
{
    using namespace ox::pipe;

    /// Focus_policy to string
    auto to_string = [](ox::Focus_policy p) -> wchar_t const* {
        switch (p) {
            using namespace ox;
            case Focus_policy::None: return L"None";
            case Focus_policy::Tab: return L"Tab";
            case Focus_policy::Click: return L"Click";
            case Focus_policy::Strong: return L"Strong";
            case Focus_policy::Direct: return L"Direct";
        }
        return L"";
    };

    /// Remove tab focus from \p p.
    auto const narrow = [](ox::Focus_policy p) {
        switch (p) {
            using namespace ox;
            case Focus_policy::None:
            case Focus_policy::Tab: return Focus_policy::None;
            case Focus_policy::Click:
            case Focus_policy::Strong: return Focus_policy::Click;
            case Focus_policy::Direct: return Focus_policy::Direct;
        }
        return ox::Focus_policy::None;
    };

    // clang-format off
    auto box_ptr =
        ox::layout::vertical
        (
            ox::hlabel(to_string(policy))
                | name("l")
                | align_center()
                | fixed_height(1)
                | ox::pipe::focus(narrow(policy)),
            ox::widget()
                | name("w")
                | ox::pipe::focus(policy)
        ) | bordered();

    box_ptr | children() | find("l")
            | on_focus_in([w = box_ptr->find_child_by_name("w")]
                    { ox::System::set_focus(*w); });

    box_ptr | children() | find("w")
            | on_focus_in( [&w = *box_ptr]{ w | walls(fg(ox::Color::Red)); })
            | on_focus_out([&w = *box_ptr]{ w | walls(fg(ox::Color::White)); });
    // clang-format on

    return box_ptr;
}

/// Build a focus app demo and return the owning pointer to it.
inline auto build_demo() -> std::unique_ptr<ox::Widget>
{
    using namespace ox;
    using namespace ox::pipe;

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
#endif  // TERMOX_DEMOS_FOCUS_FOCUS_DEMO_HPP
