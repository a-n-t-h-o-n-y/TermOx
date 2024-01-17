# Write File Widget

[`<termox/widget/widgets/write_file.hpp>`](../../../include/termox/widget/widgets/write_file.hpp)

Provides a filename `Line_edit` Widget and `Button` Widget to emit a
std::ofstream to the given filename for writing.

```cpp
template <typename Char_t = char>
class Write_file : public detail::Write_file_widgets {
   public:
    using Stream_t = std::basic_ofstream<Char_t>;

    sl::Signal<void(Stream_t&)> request;

   public:
    Write_file();
};

namespace ox::detail {

struct Write_file_widgets : HTuple<Button, Tile, Line_edit> {
    Button& save_btn         = this->get<0>();
    Tile& buffer             = this->get<1>();
    Line_edit& filename_edit = this->get<2>();

    Write_file_widgets();
};

}  // namespace ox::detail
```
