# Read File Widget

[`<termox/widget/widgets/read_file.hpp>`](../../../include/termox/widget/widgets/read_file.hpp)

Provides a filename `Line_edit` Widget and `Button` Widget to emit a
std::ifstream to the given filename for reading.

```cpp
template <typename Char_t = char>
class Read_file : public detail::Read_file_widgets {
   public:
    using Stream_t = std::basic_ifstream<Char_t>;

    sl::Signal<void(Stream_t&)> request;

   public:
    Read_file();
};

namespace ox::detail {

struct Read_file_widgets : HTuple<Button, Tile, Line_edit> {
    Button& open_btn         = this->get<0>();
    Tile& buffer             = this->get<1>();
    Line_edit& filename_edit = this->get<2>();

    Read_file_widgets();
};

}  // namespace ox::detail
```
