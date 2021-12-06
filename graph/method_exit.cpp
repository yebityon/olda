#pragma once
#include "graph.hpp"

namespace olda
{

    const std::vector<std::string> sp_method_exit_key = {
        "MethodFullName", "FileNum"};

    std::map<std::string, std::string> _parse_method_exit(const std::string method_exit)
    {
        std::map<std::string, std::string> mp = olda::parse_bytecode(method_exit);

        const std::string other = mp["other"];
        const std::vector<std::string> tmp = olda::split(other, ',');

        for (int i = 0; i < tmp.size(); ++i)
        {
            mp[sp_method_exit_key[i]] = tmp[i];
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

        assert(caller_top["MethodFullName"] == mep["MethodFullName"]);

        omni_graph.res.emplace_back(caller_top["MethodFullName"] + " <- " + mep["MethodFullName"]);

        omni_graph.g[omni_graph.vertex_stack.top()].flow_hash =
            std::hash<std::string>()(omni_graph.g[omni_graph.vertex_stack.top()].flow_str);

        const size_t child_hash = omni_graph.g[omni_graph.vertex_stack.top()].flow_hash;

        std::string in = "";
        std::string return_hash = "";

        const bool isObject = (mep.find("objectType") != mep.end());

        if (isObject)
        {
            // the returned value is object
        }
        else
        {
            const std::string primitive_value = mep["Value"];
            return_hash = std::to_string(std::hash<std::string>()(primitive_value));
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