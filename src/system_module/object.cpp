#include <system_module/event.hpp>
#include <system_module/events/child_event.hpp>
#include <system_module/events/enable_event.hpp>
#include <system_module/object.hpp>
#include <system_module/system.hpp>
#include <widget_module/widget.hpp>

#include <aml/signals/slot.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <string>

namespace mcurses {

Object::Object(Object&& rhs)
    : delete_later{std::move(rhs.delete_later)},
      enable{std::move(rhs.enable)},
      disable{std::move(rhs.disable)},
      destroyed{std::move(rhs.destroyed)},
      object_name_changed{std::move(rhs.object_name_changed)},
      object_name_{std::move(rhs.object_name_)},
      parent_{rhs.parent_},
      children_{std::move(rhs.children_)},
      event_filter_objects_{std::move(rhs.event_filter_objects_)},
      enabled_{rhs.enabled_},
      valid_{rhs.valid_} {
    for (auto& c : children_) {
        c->set_parent(this);
    }
    rhs.valid_ = false;
}

Object& Object::operator=(Object&& rhs) {
    if (!(this == &rhs)) {
        delete_later = std::move(rhs.delete_later);
        enable = std::move(rhs.enable);
        disable = std::move(rhs.disable);
        destroyed = std::move(rhs.destroyed);
        object_name_changed = std::move(rhs.object_name_changed);
        object_name_ = std::move(rhs.object_name_);
        parent_ = rhs.parent_;
        children_ = std::move(rhs.children_);
        event_filter_objects_ = std::move(rhs.event_filter_objects_);
        enabled_ = {rhs.enabled_};
        valid_ = rhs.valid_;
        for (auto& c : children_) {
            c->set_parent(this);
        }
        rhs.valid_ = false;
    }
    return *this;
}

Object::~Object() {
    if (valid_) {
        Child_event ev{Event::ChildRemoved, this};
        System::send_event(this->parent(), ev);
        destroyed(this);
    }
}

void Object::initialize() {
    // Slots
    this->delete_later = [this]() {
        System::post_event(this,
                           std::make_unique<Event>(Event::DeferredDelete));
    };
    this->delete_later.track(
        this->destroyed);  // Hack to disable slot when *this dies.

    this->enable = [this]() { this->set_enabled(true); };
    this->enable.track(this->destroyed);

    this->disable = [this]() { this->set_enabled(false); };
    this->disable.track(this->destroyed);
}

void Object::add_child(std::unique_ptr<Object> child) {
    children_.emplace_back(std::move(child));
    children_.back()->set_parent(
        this);  // if you move this, the child's parent pointer is invalid
    Child_event ev(Event::ChildAdded, this);
    System::send_event(this, ev);
}

void Object::delete_child(Object* child) {
    auto at = std::find_if(std::begin(children_), std::end(children_),
                           [&child](auto& c) { return c.get() == child; });
    if (at != std::end(children_)) {
        children_.erase(at);
    }
}

bool Object::event(const Event& event) {
    if (event.type() == Event::ChildAdded ||
        event.type() == Event::ChildPolished ||
        event.type() == Event::ChildRemoved) {
        return this->child_event(static_cast<const Child_event&>(event));
        // return event.is_accepted();
    }

    if (event.type() == Event::EnabledChange) {
        return this->enable_event(static_cast<const Enable_event&>(event));
        // return event.is_accepted();
    }

    // return event.is_accepted();
    return true;
}

bool Object::child_event(const Child_event& event) {
    Widget* parent = dynamic_cast<Widget*>(this->parent());
    if (parent != nullptr) {
        parent->update();
    }
    return true;
}

bool Object::enable_event(const Enable_event& event) {
    return true;
}

bool Object::event_filter(Object* watched, const Event& event) {
    return false;
}

void Object::install_event_filter(Object* filter_object) {
    if (filter_object == this) {
        return;
    }
    event_filter_objects_.push_back(filter_object);
}

Object* Object::parent() const {
    return parent_;
}

void Object::remove_event_filter(Object* obj) {
    auto at = std::find(std::begin(event_filter_objects_),
                        std::end(event_filter_objects_), obj);
    if (at != std::end(event_filter_objects_)) {
        event_filter_objects_.erase(at);
    }
}

void Object::set_name(const std::string& name) {
    object_name_ = name;
    object_name_changed(name);
}

void Object::set_parent(Object* parent) {
    parent_ = parent;
}

void Object::set_enabled(bool enabled) {
    enabled_ = enabled;
    Enable_event ee(enabled);
    System::send_event(this, ee);
}

std::vector<Object*> Object::children() const {
    std::vector<Object*> ret;
    std::transform(std::begin(children_), std::end(children_),
                   std::back_inserter(ret), [](auto& up) { return up.get(); });
    return ret;
}

}  // namespace mcurses
