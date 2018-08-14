#include <cppurses/widget/children_data.hpp>

#include <algorithm>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include <cppurses/system/events/child_event.hpp>
#include <cppurses/system/events/disable_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

Children_data::Children_data(Widget* parent) : parent_{parent} {}

void Children_data::add(std::unique_ptr<Widget> child) {
    if (child == nullptr) {
        return;
    }
    child->set_parent(parent_);
    children_.emplace_back(std::move(child));
}

void Children_data::insert(std::unique_ptr<Widget> child, std::size_t index) {
    if (index < children_.size()) {
        child->set_parent(parent_);
        children_.insert(std::begin(children_) + index, std::move(child));
    }
}

bool Children_data::has(Widget* child) const {
    for (const std::unique_ptr<Widget>& widg : children_) {
        if (widg.get() == child) {
            return true;
        }
    }
    return false;
}

bool Children_data::has(const std::string& name) const {
    for (const std::unique_ptr<Widget>& widg : children_) {
        if (widg->name() == name) {
            return true;
        }
    }
    return false;
}

bool Children_data::has_descendant(Widget* descendant) const {
    for (const std::unique_ptr<Widget>& widg : children_) {
        if (widg.get() == descendant || widg->children.has(descendant)) {
            return true;
        }
    }
    return false;
}

bool Children_data::has_descendant(const std::string& name) const {
    for (const std::unique_ptr<Widget>& widg : children_) {
        if (widg->name() == name || widg->children.has(name)) {
            return true;
        }
    }
    return false;
}

const std::vector<std::unique_ptr<Widget>>& Children_data::get() const {
    return children_;
}

std::vector<Widget*> Children_data::get_descendants() const {
    std::vector<Widget*> descendants;
    for (const std::unique_ptr<Widget>& child : children_) {
        descendants.push_back(child.get());
    }
    std::size_t index{0};
    while (index < descendants.size()) {
        for (const std::unique_ptr<Widget>& child :
             descendants[index]->children.get()) {
            descendants.push_back(child.get());
        }
        ++index;
    }
    return descendants;
}

std::unique_ptr<Widget> Children_data::remove(Widget* child) {
    auto found = std::find_if(std::begin(children_), std::end(children_),
                              [child](const std::unique_ptr<Widget>& widg) {
                                  return widg.get() == child;
                              });
    if (found == std::end(children_)) {
        return nullptr;
    }
    std::unique_ptr<Widget> removed = std::move(*found);
    removed->set_parent(nullptr);
    children_.erase(found);
    return removed;
}

std::unique_ptr<Widget> Children_data::remove(const std::string& name) {
    return this->remove(parent_->find_child(name));
}

}  // namespace cppurses
