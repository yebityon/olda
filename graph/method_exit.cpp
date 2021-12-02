#pragma once
#include "graph.hpp"

namespace olda
{

    const std::vector<std::string> method_exit_key = {};

    std::map<std::string, std::string> _parse_method_exit(const std::string method_exit_s)
    {

        std::vector<std::string> key_string;
        std::string s;
        auto tokens = split(method_exit_s, ',');
        auto keys = method_exit_key;

        if (method_exit_s.find("objectType=") == std::string::npos)
        {
            keys.erase(keys.begin() + 5);
        }
        if (tokens.size() != keys.size())
        {
            std::cout << method_exit_s << std::endl;
            std::cout << tokens.size() << std::endl;
            std::cout << keys.size() << std::endl;
        }
        assert(tokens.size() == keys.size());
        std::map<std::string, std::string> mp;
        for (int i = 0; i < tokens.size(); ++i)
        {
            mp[keys[i]] = tokens[i];
        }
        return mp;
    };

    void parse_method_exit(const std::string log, OmniGraph &omni_graph)
    {
        // before clculate all child hash

        auto v = omni_graph.vertex_stack.top();
        auto t = adjacent_vertices(v, omni_graph.g);

        std::map<std::string, std::string> mep = _parse_method_exit(log);

        auto caller_top = omni_graph.caller.top();

        assert(caller_top["method_fullname"] == mep["method_name"]);
        omni_graph.res.emplace_back(caller_top["method_fullname"] + " <- " + mep["method_name"]);
        omni_graph.g[omni_graph.vertex_stack.top()].flow_hash =
            std::hash<std::string>()(omni_graph.g[omni_graph.vertex_stack.top()].flow_str);
        // child hash
        const size_t child_hash = omni_graph.g[omni_graph.vertex_stack.top()].flow_hash;
        std::string in = "";
        for (auto &param : omni_graph.method_param_list[caller_top["method_fullname"]])
        {
            std::cout << param << std::endl;
        }
        const size_t param_hash = std::hash<std::string>()(std::accumulate(
            omni_graph.g[omni_graph.vertex_stack.top()].param_list.begin(),
            omni_graph.g[omni_graph.vertex_stack.top()].param_list.end(),
            in,
            [](auto &lhs, auto &rhs)
            { return lhs + rhs; }));

        omni_graph.g[omni_graph.vertex_stack.top()].param_hash = param_hash;

        // pop the method information.
        omni_graph.caller.pop();
        omni_graph.vertex_stack.pop();
        omni_graph.local_fields.pop();
        omni_graph.local_prim.pop();
        omni_graph.local_obj.pop();

        return;
    }

} // namespace olda