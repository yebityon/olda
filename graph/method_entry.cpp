#pragma once
#include "graph.hpp"

namespace olda
{

    std::map<std::string, std::string> _parse_method_entry(const std::string method_entry)
    {
        std::vector<std::string> key = {
            "event_id", "event_type", "thread_id", "data_id", "value",
            "method", "nazo_1", "class", "method", "nazo_2",
            "nazo_int", "filename", "hash", "method_fullname", "line"};
        // optional
        if (method_entry.find("VReceiver") != std::string::npos)
        {
            key.insert(key.begin() + 3, "vreceiver");
        }
        auto tokens = olda::split(method_entry, ',');

        if (tokens.size() != key.size())
        {
            std::cout << "token " << tokens.size() << std::endl;
            std::cout << "key " << key.size() << std::endl;
        }

        assert(tokens.size() == key.size());
        std::map<std::string, std::string> mp;
        for (int i = 0; i < tokens.size(); ++i)
        {
            mp[key[i]] = tokens[i];
        }
        return mp;
    }

    void parse_method_entry(const std::string log, OmniGraph &omni_graph)
    {

        assert(log.find("METHOD_ENTRY") != std::string::npos);

        omni_graph.local_fields.push(std::map<std::string, std::string>());
        omni_graph.local_prim.push(std::map<int, std::string>());
        omni_graph.local_obj.push(std::map<int, std::string>());

        std::map<std::string, std::string> mep = _parse_method_entry(log);

        assert(mep["event_type"] == "EventType=METHOD_ENTRY");

        if (omni_graph.caller.empty())
        {
            // if the caller is empty, it indicate that this vertex is Main

            omni_graph.root = add_vertex(omni_graph.g);
            omni_graph.g[omni_graph.root].method_hash = mep["hash"];
            omni_graph.g[omni_graph.root].edge_cnt += 1;
            omni_graph.g[omni_graph.root].method_str = mep["method_fullname"];
            omni_graph.g[omni_graph.root].weak_flow = omni_graph.context;
            assert(omni_graph.context.size() == 0);
            omni_graph.caller.push(mep);
            omni_graph.vertex_stack.push(omni_graph.root);
            return;
        }

        auto &prev_method = omni_graph.caller.top();
        Graph::vertex_descriptor from = omni_graph.vertex_stack.top();
        Graph::vertex_descriptor to = add_vertex(omni_graph.g);

        std::string prev_method_name = prev_method["method_fullname"];
        std::string current_method_name = mep["method_fullname"];
        // update current information
        omni_graph.g[to].method_hash = mep["hash"];
        omni_graph.g[to].method_str = current_method_name;
        omni_graph.g[to].weak_flow = omni_graph.context;
        omni_graph.g[to].weak_hash = std::hash<std::string>()(omni_graph.context);
        omni_graph.g[from].edge_cnt += 1;

        // generete edge
        bool isInserted = false;
        Graph::edge_descriptor e;
        boost::tie(e, isInserted) = add_edge(from, to, omni_graph.g);
        omni_graph.g[e].from = prev_method_name;
        omni_graph.g[e].to = current_method_name;
        omni_graph.g[e].cost = omni_graph.g[from].edge_cnt;

        omni_graph.res.emplace_back(prev_method_name + " -> " + current_method_name);
        assert(mep["event_type"] == "EventType=METHOD_ENTRY");
        omni_graph.caller.push(mep);
        omni_graph.vertex_stack.push(to);
    }

}