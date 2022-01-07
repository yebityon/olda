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

    bool dfs(Graph::vertex_descriptor vg, Graph &g,
             Graph::vertex_descriptor vu, Graph &u, Graph::vertex_descriptor diffTop, std::map<std::string, std::string> &opt, Graph &diffGraph, bool is_root)
    {
        Graph::out_edge_iterator obeg, oend;
        Graph::out_edge_iterator tbeg, tend;

        boost::tie(obeg, oend) = boost::out_edges(vg, g);
        boost::tie(tbeg, tend) = boost::out_edges(vu, u);

        //  Guarantee that there are same child
        if (not(check_range(obeg, oend) && check_range(tbeg, tend)))
        {
            // Note: ALL child methods are iterated. NO need to traversal.
            // OK
            return true;
        }

        if (not has_same_child(obeg, oend, tbeg, tend))
        {
            std::cout << "===========================================================" << std::endl;
            auto isolated_v = isorated_vertex(obeg, oend, g, tbeg, tend, u);
            std::cout << "[olda]: "
                      << "isolated vertex is defected" << std::endl;

            size_t o_size = std::distance(obeg, oend);
            size_t t_size = std::distance(tbeg, tend);

            std::cout << g[vg].method_str << " " << vg << " in origin has " << o_size;
            std::cout << " method call. but ";
            std::cout << u[vu].method_str << " " << vu << " in target has " << t_size << " method calls" << std::endl;

            // flush method call
            std::cout << "[olda]: Origin method calls" << std::endl;
            for (int i = 0; i < o_size; ++i)
            {
                std::cout << ">>>>> " << g[boost::target(*(obeg + i), g)].method_str << ": " << get_control_hash(g[boost::target(*(obeg + i), g)], opt) << std::endl;
            }

            std::cout << "[olda]: Target method calls" << std::endl;
            for (int i = 0; i < t_size; ++i)
            {
                std::cout << ">>>>> " << u[boost::target(*(tbeg + i), u)].method_str << ": " << get_control_hash(u[boost::target(*(tbeg + i), u)], opt) << std::endl;
            }

            // There are some isolated vertex inthe Graph
            std::cout << "===========================================================" << std::endl;
        }

        auto oitr = obeg;
        auto titr = tbeg;
        while (check_range(oitr, oend) && check_range(titr, tend))
        {
            auto ocv = boost::target(*oitr, g);
            auto tcv = boost::target(*titr, u);

            if (g[ocv].method_str != u[tcv].method_str)
            {
                std::cout << "[olda] : Assertion Failed" << std::endl;
                std::cout << ">>>>> " << g[ocv].method_str << "  and  " << u[tcv].method_str << " are different" << std::endl;
                return false;
            }

            if (get_control_hash(g[ocv], opt) == get_control_hash(u[tcv], opt))
            {

                // Note child vertex has completely same hash, No need to traversal
                if (check_range(oitr, oend) && check_range(titr, tend))
                {
                    ++oitr;
                    ++titr;
                }
                else
                {
                    std::cout << "[olda]: Assertion failed. Hash value is same but there are no vettex to visit" << std::endl;
                    return false;
                }
            }
            else
            {
                // Note : the hash is different...  you need to travel more.
                auto v = boost::add_vertex(diffGraph);
                diffGraph[v] = g[ocv];

                diffGraph[v].method_str = diffGraph[v].method_str +
                                          "\norigin=" + std::to_string(get_hash(g[ocv], opt)) +
                                          "\no_control=" + std::to_string(get_control_hash(g[ocv], opt)) +
                                          "\ntarget=" + std::to_string(get_hash(u[tcv], opt)) +
                                          "\nt_control=" + std::to_string(get_control_hash(u[tcv], opt));

                if (not is_root)
                {
                    Graph::edge_descriptor e;
                    bool is_inserted = false;
                    boost::tie(e, is_inserted) = boost::add_edge(diffTop, v, diffGraph);
                    diffGraph[e].cost = diff_edge_cnt++;
                }
                std::cout << g[ocv].method_str << ": " << ocv << " | " << u[tcv].method_str << ": " << tcv << std::endl;
                bool res = dfs(ocv, g, tcv, u, v, opt, diffGraph, false);
                if (not res)
                {
                    return false;
                }
                ++oitr;
                ++titr;
            }
        }
        std::cout << "something is wrong.....!" << std::endl;
        return false;
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
        diffGraph[v] = g[ov];

        dfs(ov, g, tv, u, v, opt, diffGraph, false);

        std::cout << "[olda]: DONE\n"
                  << std::endl;
        return diffGraph;
    }

    Graph backward_diff(OmniGraph origin, OmniGraph target, std::map<std::string, std::string> &opt)
    {

        std::cout << "backward_diff is called." << std::endl;

        size_t edge_g_cnt = 0, edge_u_cnt = 0;

        Graph &g = origin.g;
        Graph &u = target.g;
        auto &g_path = origin.path;
        auto &u_path = target.path;

        u_path = filter(u_path, u);
        g_path = filter(g_path, g);

        std::reverse(g_path.begin(), g_path.end());
        std::reverse(u_path.begin(), u_path.end());

        Graph diffGraph;
        Graph::vertex_descriptor root;

        std::stack<Graph::vertex_descriptor> preV;

        std::map<size_t, Graph::vertex_descriptor> hash_memo;

        Graph::vertex_iterator bgn, lst;

        // recored all hash value of vertex
        for (boost::tie(bgn, lst) = vertices(g); bgn != lst; bgn++)
        {
            if (hash_memo.find(get_hash(g[*bgn], opt)) != hash_memo.end())
            {
                //                std::cout << " ======= same vertex exist ======== " << std::endl;
            }
            hash_memo[get_hash(g[*bgn], opt)] = *bgn;
        }

        for (auto &p : u_path)
        {
            auto v = u[p];
            if (hash_memo.find(get_hash(v, opt)) != hash_memo.end() &&
                g[hash_memo.find(get_hash(v, opt))->first].method_str == v.method_str)
            {
                auto new_vertex = add_vertex(diffGraph);
                diffGraph[new_vertex] = v;

                if (not preV.empty())
                {
                    auto pv = preV.top();
                    preV.pop();

                    Graph::edge_descriptor e;
                    bool is_inserted = false;

                    boost::tie(e, is_inserted) = add_edge(pv, new_vertex, diffGraph);
                }
                preV.push(new_vertex);
            }
            else
            {
                auto new_vertex = add_vertex(diffGraph);
                diffGraph[new_vertex] = v;

                if (not preV.empty())
                {
                    auto pv = preV.top();
                    preV.pop();

                    Graph::edge_descriptor e;
                    bool is_inserted = false;

                    boost::tie(e, is_inserted) = add_edge(pv, new_vertex, diffGraph);
                }

                auto same_hash_gv = add_vertex(diffGraph);
                diffGraph[same_hash_gv] = g[hash_memo[get_hash(v, opt)]];

                std::cout << g[hash_memo[get_hash(v, opt)]].method_str << " " << v.method_str << " has same verticies !!" << std::endl;

                Graph::edge_descriptor e;
                bool is_inserted = false;

                boost::tie(e, is_inserted) = add_edge(new_vertex, same_hash_gv, diffGraph);
                diffGraph[e].cost = 12345;

                break;
            }
        }

        return diffGraph;
    }
} // namespace old