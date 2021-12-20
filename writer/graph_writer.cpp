#include "graph_writer.hpp"

namespace olda
{

    void write_graphviz(OmniGraph omni_graph, const std::string output_filename)
    {
        Graph &g = omni_graph.g;
        auto vertex_range = vertices(g);
        for (auto &first = vertex_range.first, last = vertex_range.second; first != last; ++first)
        {
            Graph::vertex_descriptor v = *first;

            const std::string flow_hash = std::to_string(g[v].flow_hash);
            const std::string param_hash = std::to_string(g[v].param_hash);

            g[v].output_format = g[v].method_str + "\nflow=" + flow_hash + "\nparam=" + param_hash;
        }

        auto edge_range = edges(g);
        for (auto first = edge_range.first, last = edge_range.second; first != last; ++first)
        {
            Graph::edge_descriptor e = *first;
        }
        /*  output dot file*/
        std::ofstream graph_dot_file(output_filename);
        boost::write_graphviz(graph_dot_file, g,
                              boost::make_label_writer(get(&method_vertex::output_format, g)),
                              boost::make_label_writer(get(&method_edges::cost, g)));
    }

    void write_diffGraph(Graph g, const std::string output_filename)
    {

        auto vertex_range = vertices(g);
        for (auto &first = vertex_range.first, last = vertex_range.second; first != last; ++first)
        {
            Graph::vertex_descriptor v = *first;
            g[v].output_format = g[v].method_str;
        }

        std::cout << "before writing diffGraph" << std::endl;

        std::ofstream graph_dot_file(output_filename);
        boost::write_graphviz(graph_dot_file, g,
                              boost::make_label_writer(get(&method_vertex::output_format, g)),
                              boost::make_label_writer(get(&method_edges::cost, g)));
    }
}