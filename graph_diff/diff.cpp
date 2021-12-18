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
            if ((not path.empty()) and path.back() == s)
            {
            }
            else
            {
                path.emplace_back(s);
            }
            path.emplace_back(t);
        }
        return path;
    }

    bool is_sameVertex(const method_vertex &lhs, const method_vertex &rhs, std::map<std::string, std::string> &opt)
    {
        return lhs.param_hash == rhs.param_hash;
    };

    size_t get_hash(const method_vertex &v, std::map<std::string, std::string> &opt){
        return v.flow_hash;
    }

        Graph diff(OmniGraph origin, OmniGraph target, std::map<std::string, std::string> &opt)
    {

        Graph &g = origin.g;
        Graph &u = target.g;

        size_t edge_g_cnt = 0, edge_u_cnt = 0;

        // get the path from boost::dfs
        auto g_visitor = my_visitor();
        auto u_visitor = my_visitor();

        // boost::depth_first_search(g, boost::visitor(g_visitor));
        // boost::depth_first_search(u, boost::visitor(u_visitor));

        // auto g_path = unzip_dfs_path(g_visitor.get_path());
        // auto u_path = unzip_dfs_path(u_visitor.get_path());

        auto &g_path = origin.path;
        auto &u_path = target.path;

        // debug
        for (auto &p : g_path)
        {
            std::cout << g[p].method_str << " -> ";
        }
        std::cout << std::endl;
        Graph diffGraph;
        Graph::vertex_descriptor root; // the root of diffGraph

        std::vector<std::vector<Graph::vertex_descriptor>> pathDiff;
        std::stack<Graph::vertex_descriptor> prevG, prevU; // the stack of vertex of G and U

        int gi = 0, ui = 0;

        while (gi < g_path.size() && ui < u_path.size())
        {

            method_vertex gv = g[g_path[gi]];
            method_vertex uv = u[u_path[ui]];

            while (gi < g_path.size() && ui < u_path.size() &&
                   is_sameVertex(g[g_path[gi]], u[u_path[ui]], opt))
            {
                // Note the vertexs are alined
                gv = g[g_path[gi]];
                uv = u[u_path[ui]];

                auto new_vertex = add_vertex(diffGraph);
                diffGraph[new_vertex] = gv;

                if (prevG.empty())
                {
                    // No need to connect
                }
                else
                {
                    auto prevVertex = prevG.top();
                    Graph::edge_descriptor e;
                    bool is_inserted = false;
                    boost::tie(e, is_inserted) = add_edge(prevVertex, new_vertex, diffGraph);
                    diffGraph[e].cost = edge_g_cnt++;
                }
                if (not prevG.empty())
                {
                    prevG.pop();
                }
                if (not prevU.empty())
                {
                    prevU.pop();
                }
                gi++;
                ui++;
                prevG.push(new_vertex);
                prevU.push(new_vertex);
            }
            // disalling here
            if (gi >= g_path.size())
            {
                for (int i = ui; i < u_path.size(); ++i)
                {
                    auto prevVertex = prevU.top();
                    auto new_vertex = add_vertex(diffGraph);
                    diffGraph[new_vertex] = u[u_path[i]];
                    Graph::edge_descriptor e;
                    bool is_inserted = false;
                    boost::tie(e, is_inserted) = add_edge(prevVertex, new_vertex, diffGraph);
                    diffGraph[e].cost = edge_u_cnt++;
                    if (not prevU.empty())
                    {
                        prevU.pop();
                    }
                    prevU.push(u_path[i]);
                }
                break;
            }

            if (ui >= u_path.size())
            {
                for (int i = gi; i < g_path.size(); ++i)
                {
                    auto prevVertex = prevG.top();
                    auto new_vertex = add_vertex(diffGraph);
                    diffGraph[new_vertex] = g[g_path[i]];

                    Graph::edge_descriptor e;
                    bool is_inserted = false;
                    boost::tie(e, is_inserted) = add_edge(prevVertex, new_vertex, diffGraph);
                    diffGraph[e].cost = edge_g_cnt++;
                    if (not prevG.empty())
                    {
                        prevG.pop();
                    }
                    prevG.push(g_path[i]);
                }
                break;
            }
            if (gi >= g_path.size() and ui >= u_path.size())
            {
                // all vertexs are itereated.
                break;
            }

            // FROM HERE, vertexs are disaligned.

            // find matching pont
            bool is_exist = false;

            for (; gi < g_path.size(); ++gi)
            {
                int u_match_idx = -1;

                for (int tui = ui; tui < u_path.size(); ++tui)
                {
                    if (is_sameVertex(g[g_path[gi]], u[u_path[tui]], opt))
                    {
                        u_match_idx = tui;
                        is_exist = true;
                        break;
                    }
                }
                if (is_exist)
                {
                    // proceed ui to u_match_idx;
                    while (ui < u_match_idx)
                    {
                        auto u_prevVertex = prevU.top();
                        auto new_vertex = add_vertex(diffGraph);
                        diffGraph[new_vertex] = u[u_path[ui]];
                        Graph::edge_descriptor e;
                        bool is_inserted = false;
                        boost::tie(e, is_inserted) = add_edge(u_prevVertex, new_vertex, diffGraph);
                        diffGraph[e].cost = edge_u_cnt++;

                        if (not prevU.empty())
                        {
                            prevU.pop();
                        }
                        prevU.push(new_vertex);
                        ++ui;
                    }
                    assert(ui == u_match_idx);

                    auto g_prevVertex = prevG.top();
                    auto new_vertex = add_vertex(diffGraph);

                    diffGraph[new_vertex] = g[g_path[gi]];

                    Graph::edge_descriptor e;
                    bool is_inserted = false;
                    boost::tie(e, is_inserted) = add_edge(g_prevVertex, new_vertex, diffGraph);
                    diffGraph[e].cost = edge_g_cnt++;

                    auto u_prevVertex = prevU.top();

                    boost::tie(e, is_inserted) = add_edge(u_prevVertex, new_vertex, diffGraph);
                    diffGraph[e].cost = edge_u_cnt++;

                    prevG.push(new_vertex);
                    prevU.push(new_vertex);

                    // insert graph
                    ui = u_match_idx + 1;
                    gi++;
                    break; // break from for loop.
                }
                else
                {
                    // same as g[g_path[gi]]
                    auto prevVertex = prevG.top();
                    auto new_vertex = add_vertex(diffGraph);
                    diffGraph[new_vertex] = g[g_path[gi]];

                    Graph::edge_descriptor e;
                    bool is_inserted = false;
                    boost::tie(e, is_inserted) = add_edge(prevVertex, new_vertex, diffGraph);
                    diffGraph[e].cost = edge_g_cnt++;
                    prevG.push(new_vertex);
                    break;
                }
            }
            // cant gling
        }
        // some path may not have been finished. make sure iterate all vertex.

        return diffGraph;
    }

    Graph easy_diff(OmniGraph origin, OmniGraph target, std::map<std::string, std::string> &opt)
    {
        std::cout << "easy_diff is called....!!\n"
                  << std::endl;

        size_t edge_g_cnt = 0, edge_u_cnt = 0;

        Graph &g = origin.g;
        Graph &u = target.g;

        auto g_visitor = my_visitor();
        auto u_visitor = my_visitor();

        auto &g_path = origin.path;
        auto &u_path = target.path;
        
        std::reverse(g_path.begin(), g_path.end());
        std::reverse(u_path.begin(), u_path.end());
        

        Graph diffGraph;
        Graph::vertex_descriptor root;
        
        std::stack<Graph::vertex_descriptor> preV;
        
        size_t idx = 0;
        
        while(idx < std::min(g_path.size(), u_path.size())){
            
            if( not is_sameVertex(g[g_path[idx]],u[u_path[idx]],opt)){

                auto new_vertex = add_vertex(diffGraph);
                diffGraph[new_vertex] = g[g_path[idx]];
                if (not preV.empty())
                {
                    auto pv = preV.top();
                    // preV.pop();

                    Graph::edge_descriptor e;
                    bool is_inserted = false;
                    boost::tie(e, is_inserted) = add_edge(pv, new_vertex, diffGraph);
                }

                new_vertex = add_vertex(diffGraph);
                diffGraph[new_vertex] = u[u_path[idx]];
                if (not preV.empty())
                {
                    auto pv = preV.top();
                    preV.pop();

                    Graph::edge_descriptor e;
                    bool is_inserted = false;
                    boost::tie(e, is_inserted) = add_edge(pv, new_vertex, diffGraph);
                }
                break; // break from loop
            }
            else
            {
                //same vertex 
                auto new_vertex = add_vertex(diffGraph);
                diffGraph[new_vertex] = g[g_path[idx]];
                
                if(not preV.empty()){
                    auto pv = preV.top();
                    preV.pop();
                    Graph::edge_descriptor e;
                    bool is_inserted = false;
                    boost::tie(e, is_inserted) = add_edge(pv, new_vertex, diffGraph);
                    
                }
                preV.push(new_vertex);
            }

            idx++;
        }

        // std::reverse(g_path.begin(), g_path.end());
        // std::reverse(u_path.begin(), u_path.end());

        // Graph &g = origin.g;
        // Graph &u = target.g;

        // std::cout << "Getthing the path is end..." << std::endl;
        // std::cout << "g_path"
        //           << " " << g_path.size() << " "
        //           << "u_path " << u_path.size() << std::endl;
        // // debug
        // Graph diffGraph;
        // Graph::vertex_descriptor root; // the root of diffGraph

        // std::vector<std::vector<Graph::vertex_descriptor>> pathDiff;
        // std::stack<Graph::vertex_descriptor> prevG, prevU; // the stack of vertex of G and U

        // int gi = 0, ui = 0;

        // while (gi < g_path.size() && ui < u_path.size())
        // {
        //     std::cout << gi << ":" << g_path.size() << " " << (gi * 100 / g_path.size()) << "% " << std::endl;
        //     while (gi < g_path.size() && ui < u_path.size() &&
        //            is_sameVertex(g[g_path[gi]], u[u_path[ui]], opt))
        //     {
        //         std::cout << gi << ":" << g_path.size() << " " << (gi * 100 / g_path.size()) << "% " << std::endl;
        //         // Note the vertexs are alined
        //         auto gv = g[g_path[gi]];
        //         auto uv = u[u_path[ui]];

        //         auto new_vertex = add_vertex(diffGraph);
        //         diffGraph[new_vertex] = gv;

        //         if (prevG.empty())
        //         {
        //             // No need to connect
        //         }
        //         else
        //         {
        //             auto prevVertex = prevG.top();
        //             Graph::edge_descriptor e;
        //             bool is_inserted = false;
        //             boost::tie(e, is_inserted) = add_edge(prevVertex, new_vertex, diffGraph);
        //             diffGraph[e].cost = edge_g_cnt++;
        //         }
        //         if (not prevG.empty())
        //         {
        //             prevG.pop();
        //         }
        //         if (not prevU.empty())
        //         {
        //             prevU.pop();
        //         }
        //         gi++;
        //         ui++;
        //         prevG.push(new_vertex);
        //         prevU.push(new_vertex);
        //     }
        //     // disalling here
        //     if (gi >= g_path.size())
        //     {
        //         for (int i = ui; i < u_path.size(); ++i)
        //         {
        //             auto prevVertex = prevU.top();
        //             auto new_vertex = add_vertex(diffGraph);
        //             diffGraph[new_vertex] = u[u_path[i]];
        //             Graph::edge_descriptor e;
        //             bool is_inserted = false;
        //             boost::tie(e, is_inserted) = add_edge(prevVertex, new_vertex, diffGraph);
        //             diffGraph[e].cost = edge_u_cnt++;
        //             if (not prevU.empty())
        //             {
        //                 prevU.pop();
        //             }
        //             prevU.push(u_path[i]);
        //         }
        //         break;
        //     }

        //     if (ui >= u_path.size())
        //     {
        //         for (int i = gi; i < g_path.size(); ++i)
        //         {
        //             auto prevVertex = prevG.top();
        //             auto new_vertex = add_vertex(diffGraph);
        //             diffGraph[new_vertex] = g[g_path[i]];

        //             Graph::edge_descriptor e;
        //             bool is_inserted = false;
        //             boost::tie(e, is_inserted) = add_edge(prevVertex, new_vertex, diffGraph);
        //             diffGraph[e].cost = edge_g_cnt++;
        //             if (not prevG.empty())
        //             {
        //                 prevG.pop();
        //             }
        //             prevG.push(g_path[i]);
        //         }
        //         break;
        //     }
        //     if (gi >= g_path.size() and ui >= u_path.size())
        //     {
        //         // all vertexs are itereated.
        //         break;
        //     }

        //     std::cout << "380 -> " << gi << ":" << g_path.size() << " " << (gi * 100 / g_path.size()) << "% " << std::endl;
        //     // different hash vertex are detected !!
        //     if (gi < g_path.size())
        //     {
        //         auto prevVertex = prevG.top();
        //         auto new_vertex = add_vertex(diffGraph);
        //         diffGraph[new_vertex] = g[g_path[gi]];

        //         Graph::edge_descriptor e;
        //         bool is_inserted = false;
        //         boost::tie(e, is_inserted) = add_edge(prevVertex, new_vertex, diffGraph);
        //         diffGraph[e].cost = edge_g_cnt++;
        //         prevG.push(new_vertex);
        //         gi += 1;
        //     }

        //     if (ui < u_path.size())
        //     {
        //         auto prevVertex = prevU.top();
        //         auto new_vertex = add_vertex(diffGraph);
        //         diffGraph[new_vertex] = u[u_path[ui]];
        //         Graph::edge_descriptor e;
        //         bool is_inserted = false;

        //         boost::tie(e, is_inserted) = add_edge(prevVertex, new_vertex, diffGraph);
        //         diffGraph[e].cost = edge_u_cnt++;
        //         prevU.push(new_vertex);
        //         ui += 1;
        //     }
        // };

        std::cout << "[olda]: succesfully calculate diff Graph...\n";

        return diffGraph;
    }

    Graph backward_diff(OmniGraph origin, OmniGraph target, std::map<std::string, std::string> &opt){
        
        std::cout << "backward_diff is called." << std::endl;
        size_t edge_g_cnt = 0, edge_u_cnt = 0;

        Graph &g = origin.g;
        Graph &u = target.g;
        auto &g_path = origin.path;
        auto &u_path = target.path;
        
        std::reverse(g_path.begin(), g_path.end());
        std::reverse(u_path.begin(), u_path.end());

        Graph diffGraph;
        Graph::vertex_descriptor root;

        std::stack<Graph::vertex_descriptor> preV;

        std:map<size_t, Graph::vertex_descriptor> hash_memo;
        
        Graph::vertex_iterator bgn, lst;
        
        // recored all hash value of vertex 
        for (boost::tie(bgn, lst) = vertices(g); beg != end; beg++)
        {
            hash_memo[get_hash(*bgn)] = *bgn;
        }
        
        for(auto& p : u_path){
            auto v =u[p];
            if( hash_memo.find(get_hash(v)) != hash_memo.end())){
                
                auto new_vertex = add_vertex(diffGraph);
                diffGraph[new_vertex] = v;

                if(not preV.empty()){
                    auto pv = preV.top();
                    preV.pop();
                    
                    Graph::edge_descriptor e;
                    bool is_inserted = false;
                    
                    boost::tie(e, is_inserted) = 
                }
                
                preV.push(v);
            } else {
                // same vertices 
                
            }
        }

        return diffGraph;
        
    }
} // namespace old