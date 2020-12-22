#include <termox/widget/detail/graph_tree.hpp>

#include <fstream>
#include <string>

#include <termox/widget/widget.hpp>

namespace {
using namespace ox;

void begin_graph(std::ofstream& file, std::string const& graph_name)
{
    file << "digraph " << graph_name << " {\n";
}

void end_graph(std::ofstream& file) { file << "}"; }

void add_connection(std::ofstream& file,
                    std::string const& from,
                    std::string const& to)
{
    file << from << " -> " << to << '\n';
}

void add_label(std::ofstream& file,
               std::string const& id,
               std::string const& label)
{
    file << id << " [label=\"" << label << "\"];\n";
}

auto make_label_text(const Widget& w) -> std::string
{
    auto const id         = std::to_string(w.unique_id());
    auto const name       = w.name();
    auto const is_enabled = w.is_enabled() ? "enabled" : "disabled";
    return id + "\n" + name + "\n" + is_enabled;
}

void make_connections_to_children(std::ofstream& file, Widget const& parent)
{
    add_label(file, std::to_string(parent.unique_id()),
              make_label_text(parent));
    if (parent.get_children().empty())
        return;
    for (auto const& child : parent.get_children()) {
        add_connection(file, std::to_string(parent.unique_id()),
                       std::to_string(child.unique_id()));
        make_connections_to_children(file, child);
    }
}

}  // namespace

namespace ox::detail {

void graph_tree(Widget const& w, std::string const& filename)
{
    auto file = std::ofstream{filename + ".gv"};
    begin_graph(file, filename);
    make_connections_to_children(file, w);
    end_graph(file);
}

}  // namespace ox::detail
