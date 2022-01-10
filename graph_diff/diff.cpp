#include "tree_diff.hpp"

namespace olda
{

    int diff_edge_cnt = 0;

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
        if (opt["param"] == "valid")
            return lhs.param_hash == rhs.param_hash;
        else
            return lhs.flow_hash == rhs.flow_hash;
    };

    size_t get_hash(const method_vertex &v, std::map<std::string, std::string> &opt)
    {
        if (opt["param"] == "valid")
            return v.param_hash;
        else
            return v.flow_hash;
    }

    size_t get_control_hash(const method_vertex &v, std::map<std::string, std::string> &opt)
    {
        if (opt["param"] == "valid")
            return v.control_param_hash;
        else
            return v.control_flow_hash;
    }

    bool has_same_child(Graph::out_edge_iterator oi, Graph::out_edge_iterator oe,
                        Graph::out_edge_iterator ti, Graph::out_edge_iterator te)
    {
        return std::distance(oi, oe) == std::distance(ti, te);
    }

    auto check_range = [&](Graph::out_edge_iterator l, Graph::out_edge_iterator r)
    {
        return l != r;
    };

    Graph::vertex_descriptor isorated_vertex(Graph::out_edge_iterator oi, Graph::out_edge_iterator oe, Graph &g,
                                             Graph::out_edge_iterator ti, Graph::out_edge_iterator te, Graph &u)
    {
        size_t o_size = std::distance(oi, oe);
        size_t t_size = std::distance(ti, te);

        if (o_size > t_size)
        {
            for (int i = 0; i < t_size; ++i)
            {
                auto o_target = boost::target(*(oi + i), g);
                auto t_target = boost::target(*(ti + i), u);

                if (o_target != t_target)
                {
                    return o_target;
                }
            }
        }
        else
        {
            for (int i = 0; i < o_size; ++i)
            {
                auto o_target = boost::target(*(oi + i), g);
                auto t_target = boost::target(*(ti + i), u);

                if (o_target != t_target)
                {
                    return t_target;
                }
            }
        }
    }

    bool synclonized_dfs(Graph::vertex_descriptor pg, Graph::vertex_descriptor cg, Graph &g,
                         Graph::vertex_descriptor pu, Graph::vertex_descriptor cu, Graph &u,
                         Graph::vertex_descriptor diffTop, std::map<std::string, std::string> &opt,
                         Graph &diffGraph, bool is_root = false)
    {
        Graph::out_edge_iterator obeg, oend;
        Graph::out_edge_iterator tbeg, tend;

        boost::tie(obeg, oend) = boost::out_edges(cg, g);
        boost::tie(tbeg, tend) = boost::out_edges(cu, u);

        // vertex validation
        if (g[cg].method_str != u[cu].method_str)
        {
            std::cout << "[olda]: diff Graph assertion. Unexpected Error is detected" << std::endl;
            std::cout << "Origin: " << g[cg].method_str << " "
                      << "Target: " << u[cu].method_str << std::endl;

            auto v1 = boost::add_vertex(diffGraph);
            auto v2 = boost::add_vertex(diffGraph);

            //

            diffGraph[v1].method_str = "DIFFERENT METHOD_CALL\nORIGIN : " + g[cg].method_str;
            diffGraph[v2].method_str = "DIFFERENT METHOD_CALL\nTARGET : " + u[cu].method_str;
            if (not is_root)
            {
                Graph::edge_descriptor e;
                bool is_inserted = false;
                boost::tie(e, is_inserted) = boost::add_edge(diffTop, v1, diffGraph);
                diffGraph[e].cost = diff_edge_cnt++;

                boost::tie(e, is_inserted) = boost::add_edge(diffTop, v2, diffGraph);
                diffGraph[e].cost = diff_edge_cnt++;
            }

            return true;
        }

        auto v = boost::add_vertex(diffGraph);
        diffGraph[v] = g[cg];

        if (not is_root)
        {
            Graph::edge_descriptor e;
            bool is_inserted = false;
            boost::tie(e, is_inserted) = boost::add_edge(diffTop, v, diffGraph);
            diffGraph[e].cost = diff_edge_cnt++;
        }

        if (get_control_hash(g[cg], opt) == get_control_hash(u[cu], opt))
        {
            // Same vertex. No need to Iterate
            diffGraph[v].method_str = diffGraph[v].method_str + "\nSame Hash";
            return false;
        }

        // Note : current vertex is Disaligned.
        if (not has_same_child(obeg, oend, tbeg, tend))
        {
            diffGraph[v].method_str = diffGraph[v].method_str + "\nDisAligned: Othe Method Call";

            auto gitr = obeg;
            auto uitr = tbeg;

            bool diff_path_found = false;

            while (gitr < oend && uitr < tend && !diff_path_found)
            {
                auto next_cg = boost::target(*gitr, g);
                auto next_cu = boost::target(*uitr, u);

                diff_path_found = synclonized_dfs(cg, next_cg, g, cu, next_cu, u, v, opt, diffGraph);

                ++gitr;
                ++uitr;
            }

            std::cout << "[olda]: "
                      << "isolated vertex is defected" << std::endl;
            size_t o_size = std::distance(gitr, oend);
            size_t t_size = std::distance(uitr, tend);

            std::cout << g[cg].method_str << " "
                      << " in origin has " << o_size;
            std::cout << " method call. but ";
            std::cout << u[cu].method_str << " "
                      << " in target has " << t_size << " method calls" << std::endl;

            // flush method call
            std::cout << "[olda]: Origin method calls" << std::endl;
            for (int i = 0; i < o_size; ++i)
            {
                std::cout << ">>>>> " << g[boost::target(*(gitr + i), g)].method_str << ": " << get_control_hash(g[boost::target(*(gitr + i), g)], opt) << std::endl;
            }

            std::cout << "[olda]: Target method calls" << std::endl;
            for (int i = 0; i < t_size; ++i)
            {
                std::cout << ">>>>> " << u[boost::target(*(uitr + i), u)].method_str << ": " << get_control_hash(u[boost::target(*(uitr + i), u)], opt) << std::endl;
            }

            return true;
        }

        diffGraph[v].method_str = diffGraph[v].method_str +
                                  "\noparam=" + std::to_string(g[cg].param_hash) +
                                  "\noflow=" + std::to_string(g[cg].flow_hash) +
                                  "\nocparam=" + std::to_string(g[cg].control_param_hash) +
                                  "\nocflow=" + std::to_string(g[cg].control_flow_hash) +
                                  "\ntparam=" + std::to_string(u[cu].param_hash) +
                                  "\ntflow=" + std::to_string(u[cu].flow_hash) +
                                  "\ntcparam=" + std::to_string(u[cu].control_param_hash) +
                                  "\ntcflow=" + std::to_string(u[cu].control_flow_hash);

        // ccurrent vertex
        if (std::distance(obeg, oend) == 0)
        {
            assert(std::distance(tbeg, tend) == 0);
            return get_control_hash(g[cg], opt) == get_control_hash(u[cu], opt);
        }

        // this vertex is leaf, aligned and same child number

        auto gitr = obeg;
        auto uitr = tbeg;

        assert(std::distance(gitr, oend) == std::distance(uitr, tend));

        bool diff_path_found = false;

        while (gitr < oend && uitr < tend && !diff_path_found)
        {
            auto next_cg = boost::target(*gitr, g);
            auto next_cu = boost::target(*uitr, u);

            diff_path_found = synclonized_dfs(cg, next_cg, g, cu, next_cu, u, v, opt, diffGraph);

            ++gitr;
            ++uitr;
        }

        // Note: All of are iterated, return true to teminate.
        return true;
    }

    std::vector<Graph::vertex_descriptor> filter(std::vector<Graph::vertex_descriptor> &path, Graph &g)
    {
        std::vector<Graph::vertex_descriptor> res;
        for (auto &p : path)
        {
            const std::string method_name = g[p].method_str;
            if (method_name.find("junit") == std::string::npos &&
                method_name.find("ant") == std::string::npos)
                res.push_back(p);
        }
        return res;
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

    /*
     *  Easy diff return the subset of the vertex. these vertex has different  hash value betwenn Origin and Target.
     *
     */

    Graph easy_diff(OmniGraph origin, OmniGraph target, std::map<std::string, std::string> &opt)
    {

        std::cout << "[olda]: easy_diff is runnning...!\n";

        Graph &g = origin.g;
        Graph &u = target.g;
        Graph diffGraph;

        // std::stack<Graph::vertex_descriptor> diff_path;

        auto ov = origin.root;
        auto tv = target.root;

        std::cout << "root: " << g[origin.root].method_str << ": " << origin.root << std::endl;
        std::cout << "root: " << u[target.root].method_str << ": " << target.root << std::endl;

        if (get_control_hash(g[origin.root], opt) == get_control_hash(u[target.root], opt))
        {
            std::cout << "[olda]: Completly Same Verticies" << std::endl;
            exit(0);
        }

        auto v = boost::add_vertex(diffGraph);
        diffGraph[v].method_str = "DiffGraph of Olda";
        synclonized_dfs(ov, ov, g, tv, tv, u, v, opt, diffGraph, true);

        std::cout << "[olda]: DONE\n"
                  << std::endl;
        return diffGraph;
    }
} // namespace old