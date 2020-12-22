# Border Design Document - Part I

## Brief Description

Borders will no longer be stored as an object within the Widget class, but will
be their own Widget type that owns a single Widget that it wraps around. This
makes Border a Layout type, technically.

## Rationale

Border objects are currently stored within the Widget class. But not all Widgets
use Borders, and Borders are actually outside of Widgets, yet the Widget
contains the Border object within itself.

There is a better way, where a Border is its own Widget that wraps around
another Widget. This cleans up a bit of awkward code within Widget, where to get
the dimensions or location of the Widget, you have to specify if those are with
or without the Border, and it is some calculation to figure out how much Border
space is being used to determine the Widget's actual location.

This also opens up the design to have specializations for when a border is
wrapping a Layout type, for instance, a `layout::Horizontal` object can have
intelligent border drawing between each widget, with the correct characters
displayed to tie it together. This is not possible with the current design,
where a Border only knows about the Widget that owns it.

## Interface

### `class layout::Fixed`

```cpp
/// Holds a single Widget with offsets from each wall.
template <typename Widget_t>
class Fixed : public layout::Layout<Widget_t> {
   public:
    /// Creates a new Fixed layout with child Widget_t.
    /** \p args are forwarded to the Widget_t constructor. */
    template <typename... Args>
    Fixed(Args&&... args);

   public:
    /// Set the child Widget offsets all at once.
    this could take an Offset type, if you make it public, then the single
    letter names are not an issue.
    void set_offsets(std::size_t n, std::size_t s, std::size_t e, std::size_t w);

    /// Set the child offset from the north wall.
    void set_north_offset(std::size_t offset);

    /// Set the child offset from the south wall.
    void set_south_offset(std::size_t offset);

    /// Set the child offset from the east wall.
    void set_east_offset(std::size_t offset);

    /// Set the child offset from the west wall.
    void set_west_offset(std::size_t offset);

   public:
    /// return a const reference to the child Widget.
    auto child() const -> Widget_t const&;

    /// return a reference to the child Widget.
    auto child() -> Widget_t&;

    // replace the child widget with a new widget? or not!

    protected:
    /// Send move and resize events when needed.
    void update_geometry() override;

   private:
    struct Offsets {
        std::size_t north = 0;
        std::size_t south = 0;
        std::size_t east  = 0;
        std::size_t west  = 0;
    } offsets_;

   private:
    using Layout::get_children;
    using Layout::insert_child;
    using Layout::append_child;
    using Layout::remove_child;
    using Layout::remove_child_if;
    using Layout::remove_child_at;
    using Layout::remove_and_delete_child;
    using Layout::remove_and_delete_child_if;
    using Layout::remove_and_delete_child_at;
    using Layout::delete_all_children;
    using Layout::swap_children;
    using Layout::find_child_if;
    using Layout::find_child_by_name;
    using Layout::find_child_position;
    using Layout::contains_child;
    using Layout::contains_descendant;
};
```

- Layout::Fixed has offsets for each side and owns a single Widget, with hidden
  Layout type methods so the interface makes sense for a single child Widget,
  provide direct access to the widget. Template parameter is required, you can't
  change the child widget once you set it, unless you access it via a member
  function that returns a reference and you hold a pointer to the child.
  Template parameter is still required.
  Think about initialization, do you have to construct the child widget? can it
  ever not have a child widget? probably restrict that. copy and move things are
  all disabled for Widget, so don't worry about those.

- Border is a layout::Fixed with special painting for the border.

- Bordered<> is a type that is a Border that owns a type T Widget and provides
  access to it, this might be the same as Border..

- a border prototype method, this will be important to test if a border is a
  good idea or not. Its a layout so this is standard. Also make sure to take
  only a single paramter.

```cpp
template <typename W>
auto border(std::unique_ptr<W>) -> std::unique_ptr<Bordered<W>>;
```

## Client Code Examples

```cpp
class Bordered_textbox : public ox::Border<ox::Textbox> {
   public:
    Bordered_textbox() { *this | ox::pipe::bold_walls(); }
};

class Bordered_settings : public ox::Border<Settings_pane> {
   public:
    Bordered_settings()
    {
        using namespace ox::pipe;
        *this | west_border() | bold_walls();
    }
};

class Notepad : public ox::layout::Horizontal<> {
   public:
    Notepad()
    {
        settings_.bg_set.connect([this](Color c){ textbox_ | bg(c); });
    }

   private:
    ox::Textbox& textbox_ = this->make_child<Bordered_textbox>("init").child();
    Settings_pane& settings_    = this->make_child<Bordered_settings>().child();
};

```

## Part II

Part II of the design will modify `Linear_layouts` to hold a buffer space between
each widget, this is a generalization of the existing implementation which just
uses a buffer of zero between each widget.

Once you have the buffer in, you can have a `Border<layout::Horizontal>` etc...
specialization, which paints in between the widgets a border. Might want to keep
it limited to a single space between widgets for the time being, at least for
the Border class, though you don't really control that, but you could via the
Border interface.

You'll need a unique interface for this Border specialization, one that hold a
Segment for the new sections of the border that can appear.

You can also include a `layout::Float` which doesn't have anything to do with
Borders, but where the size hint in each dimension is used(as if fixed size
policy) and the rest is empty space, you can even use a stretch factor for each
side, which seems like it'd be easy enough to implement since it is know there
are only two places where buffer space can be allocated to. This type could lead
to a new look of some apps, where buttons are off on their own instead of all
stacked up. More like other terminal UIs that you see, might be able to
replicate some of those older designs.

Another layout type could be `layout::Grid` which has a size, which is the
number of widgets in the two dimensions, each row has a height and each column
has a width. Widgets do not have to be constructed in each space. Access to
widgets is through the template type provided, just like existing layouts. Yes,
this can be done with a vertical layout of horizontal layouts, but that
interface is bad, and not efficient. Another advantage of this is that you can
have a Border<layout::Grid> specialization that paints between each Widget in
the grid. This means you need a buffer parameter for the Grid layout, so that
each Widget can have a particular amount of space between itself and other.

## Other Design Ideas

- no move events, Widgets do not need to know about where they are.

- layouts hold the offset of their children from their top left, instead of
  widgets themselves holding this information. Widgets shouldn't need to know
  where they are globally. This means you can remove the `Move_event` type and
  just have layouts update their internally held offset value. The question here
  is does this offset vector need to be accessible from within Widget base class
  or can it stay in Layout? if it is unique to each layout type then it might
  need to be a virtual method, but for something like grid the interface changes
  to needed two parameters for each coordinate. So access to these offsets by
  the public might be tricky, it might be possible to keep this information
  internal only, and then have whatever utility is needed be provided by the
  layout class, abstract the data by instead only providing its use through
  member functions that are virtual and make sense for all Layouts. Not sure
  about this yet.

  Thinking a bit more about this, there are two cases where you need to know the
  offsets outside of the layout type:

  - Scanning the Widget tree from head, to paint each Widget, if need be. you
    start with an offset of zero, zero for the head Widget, you then get the
    height and width offset of each child from the head widget, so if you tie
    this height/width offset to the child, then you could have a children vector
    that doesn't just hold unique_ptr<Widget>, but instead holds a pair. so the
    coordinate offset can be held with the Widget, then when you get the widgets
    you get the offset of the parent and you don't need to worry about ordering
    or anything of access to the children. That works, but it makes access to
    the children via a simple reference more difficult. Its actually a transform
    view, or a projection, so this should be really easy to update, you just
    point it to the second element in the pair or something, and you got it. You
    may need to provide protected access within a layout to the full structure
    if you don't already.

  - Finding a widget that is at some global coordinate, for mouse clicks. This
    is simple enough with the above solution. You can still access to offsets
    from each child, you'll need to access their width/height as well, but that
    is simple enough.

- Widgets should not hold a parent pointer, that is too much information.
  This will mess with posting child polished events by the size policies I
  believe.

- Paint events should return an actual object describing the new state. This is
  just a simple map of points and glyphs, the points are local to the Widget.
  This object can be called a `Canvas` and the paint event function call takes a
  Canvas as a parameter: `paint_event(Canvas) -> Canvas` the one that is gets is
  added to, so for the call to the base class paint event, you make your paints,
  then you pass your object down to the base class, you might have to std::move
  it, which is a bit of a pain, but not much, then you just return that function
  call:

  ```cpp
  auto paint_event(Canvas c) const -> Canvas
  {
    c.put(L'X' | bg(Color::Blue), {5, 2});
    return Widget::paint_event(std::move(c));
  }
  ```

  You could delete the copy constructor if you want to guarantee you aren't
  making unnecessary copies. Taking by reference is another option, and the
  system calling the paint event method holds it on its stack, that removes the
  std::move call, but is messier, passing some random lifetime around.

  Also, paint event should be const, if you need to modify some variable, then
  it should be a mutable, list `std::size_t count_;` or something. paint event
  really shouldn't be modifying the state of the Widget. This gets it towards
  enforcement of being a pure function, can't completely stop side effects by a
  client, but you can encourage not to.

- Paint events are no longer part of the event queue, instead a call to
  `Widget::update()` will set a boolean flag to tell some other component that
  it needs to be painted, the next time painting occurs. Then after each
  invocation of the event queue being processed by the main event loop, you scan
  the entire apps widget tree, starting from head and not going down branches
  where the parent is disabled. At each widget, you check if the paint boolean
  flag is set, if it is, then you just have to call the `paint_event()` method
  of that Widget. The returned Canvas object of the paint event can then be used
  to immediately paint to the screen at the offset of itself plus a running
  total of the parent offset from the terminal screen that is kept as it is
  scanning the tree.

- The event queue can be a separate library, even if it is small. You'd have to
  figure out how to make it generic enough and efficient if you allow ordering
  somehow. Ordering of event processing is the biggest thing.  Imagining an
  interface something like this:

  ```cpp
  auto queue = mq::Message_queue<Message_variant_type>{};
  queue.append(Resize_event{{4, 3}, {2, 3}});

  // could just use iterators from the Message_queue, so unattached to the class
  mq::process(queue); // calls std::visit with process(Message_variant_type)
  queue.clear();
  ```

  So in this interface you have a user provided variant type that can be passed
  to std::visit() and have a free function called process() invoked on it. If
  the process function is in the same namespace as the variant message type,
  then you are good to go, no namespace needed in the mq::process code.
  The tricky part is in the `Message_queue` class, you'll want to provide some
  nice way of having it be a priority queue perhaps, but you'll want it to be
  quick to insert a new element and to iterate over, and to clear.
  So its not much of a library, it is just a container of variants with some
  special interface to insert and a process method that just calls std::visit
  with some generic lambda to call process() on each Message type. Still useful
  though, providing a break, a wall between the message queue and termox is
  benefical in a psychological way, it simplifies things.
