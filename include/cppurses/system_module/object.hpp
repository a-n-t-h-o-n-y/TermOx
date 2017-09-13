#ifndef SYSTEM_MODULE_OBJECT_HPP
#define SYSTEM_MODULE_OBJECT_HPP

#include <signals/signal.hpp>
#include <signals/slot.hpp>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>

namespace cppurses {

class Event;
class Child_event;
class Enable_event;

class Object {
   public:
    Object() { this->Object::initialize(); }
    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;
    Object(Object&& rhs) noexcept;
    Object& operator=(Object&& rhs) noexcept;
    virtual ~Object();

    template <typename T, typename... Args>
    T& make_child(Args&&... args) {
        this->add_child(std::make_unique<T>(std::forward<Args>(args)...));
        return static_cast<T&>(*children_.back());
    }

    void add_child(std::unique_ptr<Object> child);
    std::vector<Object*> children() const;

    template <typename T>
    T* find_child(const std::string& name) {
        return this->find_child_impl<T>(this, name);
    }

    template <typename T>
    const T* find_child(const std::string& name) const {
        return this->find_child_impl<T>(this, name);
    }

    Object* parent() const;
    void set_parent(Object* parent);

    std::string name() const { return object_name_; }
    void set_name(const std::string& name);

    virtual bool event(const Event& event);
    virtual bool event_filter(Object* watched, const Event& event);

    void install_event_filter(Object* filter_object);
    void remove_event_filter(Object* obj);

    bool enabled() const { return enabled_; }
    void set_enabled(bool enabled);

    virtual bool has_coordinates(std::size_t glob_x, std::size_t glob_y) {
        return false;
    }

    // Slots
    sig::Slot<void()> delete_later;
    sig::Slot<void()> enable;
    sig::Slot<void()> disable;

    // Signals
    sig::Signal<void(Object*)> destroyed;
    sig::Signal<void(const std::string&)> object_name_changed;

    friend class System;

   protected:
    virtual bool child_event(const Child_event& event);
    virtual bool enable_event(const Enable_event& event);

    std::string object_name_;
    Object* parent_ = nullptr;
    std::vector<std::unique_ptr<Object>> children_;
    std::vector<Object*> event_filter_objects_;
    bool enabled_ = true;

   private:
    // Breadth First Search for name
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

}  // namespace cppurses
#endif  // SYSTEM_MODULE_OBJECT_HPP
