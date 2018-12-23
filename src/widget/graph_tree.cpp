#include <cppurses/widget/detail/graph_tree.hpp>

#include <fstream>
#include <string>

#include <utility/type_info.hpp>

#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;

void begin_graph(std::ofstream& file, const std::string& graph_name) {
    file << "digraph " << graph_name << " {\n";
}

void end_graph(std::ofstream& file) {
    file << "}";
}

void add_connection(std::ofstream& file,
                    const std::string& from,
                    const std::string& to) {
    file << from << " -> " << to << '\n';
}

void add_label(std::ofstream& file,
               const std::string& id,
               const std::string& label) {
    file << id << " [label=\"" << label << "\"];\n";
}

std::string make_label_text(const Widget& w) {
    // auto type_name = std::string{utility::get_type_info<decltype(w)>()};
    auto id = std::to_string(w.unique_id());
    auto name = w.name();
    auto is_enabled = w.enabled() ? "enabled" : "disabled";
    // return type_name + " " + id + " " + name;
    return id + "\n" + name + "\n" + is_enabled;
}

void make_connections_to_children(std::ofstream& file, const Widget& parent) {
    add_label(file, std::to_string(parent.unique_id()),
              make_label_text(parent));
    if (parent.children.get().empty()) {
        return;
    }
    for (const auto& child : parent.children.get()) {
        add_connection(file, std::to_string(parent.unique_id()),
                       std::to_string(child->unique_id()));
        make_connections_to_children(file, *child);
    }
}

}  // namespace

namespace cppurses {
namespace detail {

/// Outputs filename.gz graph description of widget tree hierarchy.
void graph_tree(const Widget& w, const std::string& filename) {
    auto file = std::ofstream{filename + ".gv"};
    begin_graph(file, filename);
    make_connections_to_children(file, w);
    end_graph(file);
}

}  // namespace detail
}  // namespace cppurses
