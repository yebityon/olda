#include "tree_diff.hpp"

namespace olda
{

    // visitor class for dfs
    class my_visitor : public boost::default_dfs_visitor
    {
    public:
        my_visitor() : path(new std::vector<std::pair<Graph::vertex_descriptor, Graph::vertex_descriptor>>()) {}
        int allcnt = 0;
        template <class Edge, class Graph>
        void examine_edge(Edge e, Graph g)
        {
            auto s = source(e, g);
            auto t = target(e, g);

            path->emplace_back(s, t);
        }
        template <class Vertex, class Graph>
        // func is called when it found new vertex
        void start_vertex(Vertex v, Graph g)
        {
            g[v].id = allcnt++;
        }
        std::vector<std::pair<Graph::vertex_descriptor, Graph::vertex_descriptor>> &get_path() const { return *path; }

    private:
        boost::shared_ptr<std::vector<std::pair<Graph::vertex_descriptor, Graph::vertex_descriptor>>> path;
    };

    std::vector<Graph::vertex_descriptor> unzip_dfs_path(
        std::vector<std::pair<Graph::vertex_descriptor, Graph::vertex_descriptor>> &p)
        // Note this function can be removed.
    {

        std::vector<Graph::vertex_descriptor> path;
        for (auto &[s, t] : p)
        {
            if((not path.empty()) and path.back() == s){
                
            } else {
                path.emplace_back(t);
            }
        }
        return path;
        
    }

    bool is_sameVertex(const method_vertex& lhs, const method_vertex& rhs,std::map<std::string,std::string>&opt){

        return lhs.flow_hash == rhs.flow_hash;
    };
    
    Graph diff(OmniGraph origin, OmniGraph target, std::map<std::string, std::string> &opt)
    {

        Graph &g = origin.g;
        Graph &u = target.g;

        // get the path from boost::dfs
        auto g_visitor = my_visitor();
        auto u_visitor = my_visitor();

        boost::depth_first_search(g, boost::visitor(g_visitor));
        boost::depth_first_search(u, boost::visitor(u_visitor));

        auto g_path = unzip_dfs_path(g_visitor.get_path());
        auto u_path = unzip_dfs_path(u_visitor.get_path());

        Graph diffGraph;
        Graph::vertex_descriptor root; // the root of diffGraph

        std::vector<std::vector<Graph::vertex_descriptor>> pathDiff;
        std::stack<Graph::vertex_descriptor> prevG, prevU; // the stack of vertex of G and U

        int gi = 0, ui = 0;

        while (gi < g_path.size() && ui < u_path.size())
        {

            method_vertex gv = g[g_path[gi]];
            method_vertex uv = u[u_path[ui]];

            while( gi < g_path.size() && ui < u_path.size() && is_sameVertex(gv,uv,opt)){
                
            }
        }
        // some path may not have been finished. make sure iterate all vertex.
    }
}