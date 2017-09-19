#include "system/object.hpp"
#include "system/event.hpp"
#include "system/events/child_event.hpp"
#include "system/events/enable_event.hpp"
#include "system/system.hpp"
#include <algorithm>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace cppurses {

// These need to be defaulted, figure a way to set parents elsewhere.
// Object::Object(Object&& rhs) noexcept
//     : delete_later{std::move(rhs.delete_later)},
//       enable{std::move(rhs.enable)},
//       disable{std::move(rhs.disable)},
//       destroyed{std::move(rhs.destroyed)},
//       object_name_changed{std::move(rhs.object_name_changed)},
//       object_name_{std::move(rhs.object_name_)},
//       parent_{rhs.parent_},
//       children_{std::move(rhs.children_)},
//       event_filters_{std::move(rhs.event_filters_)},
//       enabled_{rhs.enabled_},
//       valid_{rhs.valid_} {
//     for (auto& c : children_) {  // Possibly put this in move ctor of a children
//         c->set_parent(this);     // container object.
//     }
//     rhs.valid_ = false;
// }

// Object& Object::operator=(Object&& rhs) noexcept {
//     if (!(this == &rhs)) {
//         delete_later = std::move(rhs.delete_later);
//         enable = std::move(rhs.enable);
//         disable = std::move(rhs.disable);
//         destroyed = std::move(rhs.destroyed);
//         object_name_changed = std::move(rhs.object_name_changed);
//         object_name_ = std::move(rhs.object_name_);
//         parent_ = rhs.parent_;
//         children_ = std::move(rhs.children_);
//         event_filters_ = std::move(rhs.event_filters_);
//         enabled_ = {rhs.enabled_};
//         valid_ = rhs.valid_;
//         for (auto& c : children_) {
//             c->set_parent(this);
//         }
//         rhs.valid_ = false;
//     }
//     return *this;
// }

// Object::~Object() {
//     if (valid_) {
//         Object* parent = this->parent();
//         if (parent != nullptr) {
//             System::send_event(Child_removed_event{static_cast<Widget*>parent, this});
//         }
//         destroyed(this);
//     }
// }

// void Object::initialize() {
//     // Slots
//     this->delete_later = [this] {
//         System::post_event(this,
//                            std::make_unique<Event>(Event::DeferredDelete));
//     };
//     this->delete_later.track(this->destroyed);

//     this->enable = [this] { this->set_enabled(true); };
//     this->enable.track(this->destroyed);

//     this->disable = [this] { this->set_enabled(false); };
//     this->disable.track(this->destroyed);
// }

// void Object::add_child(std::unique_ptr<Object> child) {
//     children_.emplace_back(std::move(child));
//     children_.back()->set_parent(this);
//     System::post_event(this,
//                        std::make_unique<Child_event>(Event::ChildAdded, this));
// }

// void Object::delete_child(Object* child) {
//     auto end_iter = std::end(children_);
//     auto at = std::find_if(std::begin(children_), end_iter,
//                            [child](auto& c) { return c.get() == child; });
//     if (at != end_iter) {
//         children_.erase(at);
//         System::post_event(
//             this, std::make_unique<Child_event>(Event::ChildRemoved, this));
//     }
// }

// bool Object::event(const Event& event) {
//     auto event_t = event.type();
//     if (event_t == Event::ChildAdded || event_t == Event::ChildPolished ||
//         event_t == Event::ChildRemoved) {
//         return this->child_event(static_cast<const Child_event&>(event));
//     }
//     if (event_t == Event::EnabledChange) {
//         return this->enable_event(static_cast<const Enable_event&>(event));
//     }
//     return true;
// }

// bool Object::child_added_event(Widget* child) {
//     return true;
// }

// bool Object::child_removed_event(Widget* child) {
//     return true;
// }

// bool Object::child_polished_event(Widget* child) {
//     return true;
// }

// bool Object::enable_event() {
//     return true;
// }

// bool Object::disable_event() {
//     return true;
// }

// bool Object::event_filter(Object* watched, const Event& event) {
//     return false;
// }

// void Object::install_event_filter(Object* filter_object) {  // raw ptrs
//                                                             // dangerous, no
//                                                             // automatic storage
//                                                             // safety here
//     if (filter_object == this) {
//         return;
//     }
//     event_filters_.push_back(filter_object);
// }

// Object* Object::parent() const {
//     return parent_;
// }

// void Object::remove_event_filter(Object* obj) {
//     auto at =
//         std::find(std::begin(event_filters_), std::end(event_filters_), obj);
//     if (at != std::end(event_filters_)) {
//         event_filters_.erase(at);
//     }
// }

// void Object::set_name(const std::string& name) {
//     object_name_ = name;
//     object_name_changed(name);
// }

// void Object::set_parent(Object* parent) {
//     parent_ = parent;
// }

// void Object::set_enabled(bool enabled) {
//     enabled_ = enabled;
//     if (enabled) {
//         System::post_event(std::make_unique<Enable_event>(this));
//     } else {
//         System::post_event(std::make_unique<Disable_event>(this));
//     }
// }

// std::vector<Object*> Object::children() const {
//     std::vector<Object*> ret;
//     std::transform(std::begin(children_), std::end(children_),
//                    std::back_inserter(ret), [](auto& up) { return up.get(); });
//     return ret;
// }

}  // namespace cppurses
