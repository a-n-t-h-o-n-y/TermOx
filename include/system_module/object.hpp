#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <aml/signals/signal.hpp>
#include <aml/signals/slot.hpp>

#include <algorithm>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>

namespace mcurses {

class Event;
class Child_event;
class Enable_event;

class Object {
   public:
    Object() { initialize(); }

    Object(Object&& rhs) noexcept(false);

    Object& operator=(Object&& rhs) noexcept(false);

    virtual ~Object();

    template <typename T, typename... Args>
    T& make_child(Args&&... args) {
        this->add_child(std::make_unique<T>(std::forward<Args>(args)...));
        return static_cast<T&>(*children_.back());
    }

    void add_child(std::unique_ptr<Object> child);

    virtual bool event(Event& event);

    virtual bool event_filter(Object* watched, Event& event);

    // Breadth First Search for name
    template <typename T>
    T* find_child(const std::string& name) {
        return this->find_child_impl<T>(this, name);
    }

    template <typename T>
    const T* find_child(const std::string& name) const {
        return this->find_child_impl<T>(this, name);
    }

    void install_event_filter(Object* filter_object);

    std::string name() const { return object_name_; }

    Object* parent() const;

    void remove_event_filter(Object* obj);

    void set_name(const std::string& name);

    void set_parent(Object* parent);

    void set_enabled(bool enabled);

    std::vector<Object*> children() const;

    virtual bool has_coordinates(std::size_t glob_x, std::size_t glob_y) {
        return false;
    }

    bool is_enabled() const { return enabled_; }

    // Slot - no noexcept specification for Slot's move constructor/assignment
    Slot<void()> delete_later;
    Slot<void()> enable;
    Slot<void()> disable;

    // Signals
    Signal<void(Object*)> destroyed;
    Signal<void(const std::string&)> object_name_changed;

    friend class System;

   protected:
    virtual void child_event(Child_event& event);
    virtual void enable_event(Enable_event& event);

    std::string object_name_;
    Object* parent_ = nullptr;
    std::vector<std::unique_ptr<Object>> children_;
    std::vector<Object*> event_filter_objects_;

    bool enabled_ = true;

   private:
    template <typename T, typename U>
    static auto find_child_impl(U u, const std::string& name)
        -> decltype(u->template find_child<T>(name)) {
        std::queue<U> queue_;
        queue_.push(u);
        while (!queue_.empty()) {
            auto current = queue_.front();
            queue_.pop();
            auto c_ptr = dynamic_cast<T*>(current);
            if (current->name() == name && c_ptr) {
                return c_ptr;
            }
            auto children = current->children();
            std::for_each(std::begin(children), std::end(children),
                          [&](Object* p) { queue_.push(p); });
        }
        return nullptr;
    }

    void delete_child(Object* child);
    void initialize();

    bool valid_ = true;
};

}  // namespace mcurses
#endif  // OBJECT_HPP
