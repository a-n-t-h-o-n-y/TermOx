# Focus Policy

A `Focus_policy` is an enum defining how a Widget can receive system focus for
key press events.

- `None`   - Widget cannot receive focus.
- `Tab`    - `Key::Tab` presses will cycle the focus to Widgets with this
  policy. Tab focus only works if the Widget currently in focus has a Tab
  policy. `Key::Back_tab` will cycle in the opposite direction. Focus is cycles
  in a depth-first order through the Widget tree.
- `Click`  - A left-mouse click on this Widget will give it focus.
- `Strong` - Both Tab and Click policies apply.
- `Direct` - Can only get focus if directly set with `System::set_focus(...)`

A focus policy can be set on a Widget by directly assigning to the
`Widget::focus_policy` member.

## Pipe Methods

A Focus Policy can be set using the `pipe` namespace methods.

- `focus(Focus_policy)`
- `no_focus()`
- `tab_focus()`
- `click_focus()`
- `strong_focus()`
- `direct_focus()`
