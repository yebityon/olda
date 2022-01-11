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

    std::map<std::string, std::string> _parse_call_exit(const std::string log)
    {
        auto res = olda::parse_bytecode(log);
        return res;
    }

    void parse_method_exit(const std::string log, OmniGraph &omni_graph)
    {
        // before clculate all child hash
        std::map<std::string, std::string> mep = _parse_method_exit(log);

        const int thread = std::stoi(mep["ThreadId"]);

        auto &caller = omni_graph.caller[thread];
        auto &vertex_stack = omni_graph.vertex_stack[thread];

        auto v = vertex_stack.top();
        auto t = adjacent_vertices(v, omni_graph.g);

        auto caller_top = caller.top();

        if (caller_top["MethodFullName"] != mep["MethodFullName"])
        {
            // Note CALL method
            // something Invalid. Skip the vertex
            caller.pop();
            vertex_stack.pop();
            omni_graph.local_fields.pop();
            omni_graph.local_prim.pop();
            omni_graph.local_obj.pop();
            if (caller.empty() || caller.top()["MethodFullName"] != mep["MethodFullName"])
            {
                std::cout << "Assertion failed:";
                std::cout << caller_top["MethodFullName"] << " | " << mep["MethodFullName"] << std::endl;
                return;
            }
        };

        size_t return_hash = 0;

        const bool isObject = (mep.find("objectType") != mep.end());

        if (isObject)
        {
            // the returned value is object
            // TODO : if the obj is string, just add primitive value
            const int object_id = std::stoi(mep["Value"]);
            const size_t object_hash = std::hash<std::string>()(omni_graph.object_order[object_id][-1]);
            return_hash = object_hash;
        }
        else
        {
            const std::string primitive_value = mep["Value"];
            return_hash = std::hash<std::string>()(primitive_value);
        }

        const size_t param_hash = std::hash<std::string>()(std::accumulate(
            omni_graph.g[vertex_stack.top()].param_list.begin(),
            omni_graph.g[vertex_stack.top()].param_list.end(),
            std::to_string(return_hash),
            [](auto &lhs, auto &rhs)
            { return lhs + rhs; }));

        const std::string flow_str = omni_graph.g[vertex_stack.top()].flow_str;

        const size_t flow_hash = std::hash<std::string>()(flow_str);

        const size_t control_flow_hash = std::hash<std::string>()(std::to_string(flow_hash) + std::to_string(omni_graph.g[vertex_stack.top()].control_flow_hash));

        const size_t control_param_hash = std::hash<std::string>()(std::to_string(param_hash) + std::to_string(omni_graph.g[vertex_stack.top()].control_param_hash));

        omni_graph.g[vertex_stack.top()].param_hash = param_hash;
        omni_graph.g[vertex_stack.top()].flow_hash = flow_hash;
        omni_graph.g[vertex_stack.top()].control_flow_hash = control_flow_hash;

        omni_graph.g[vertex_stack.top()].control_param_hash = control_param_hash;

        // pop the method information.
        auto id = vertex_stack.top();
        caller.pop();
        vertex_stack.pop();

        if (not vertex_stack.empty())
        {
            if (log.find("Optimizer:doOptimize") != std::string::npos || log.find("Optimizer:optimize") != std::string::npos)
            {
                std::cout << "################################################################" << std::endl;

                std::cout << "caller: " << omni_graph.g[vertex_stack.top()].method_str << " id:" << vertex_stack.top() << std::endl;
                std::cout << "callee: " << mep["MethodFullName"] << " id:" << id << std::endl;

                std::cout << "################################################################" << std::endl;
            }

            omni_graph.g[vertex_stack.top()].control_flow_str +=
                std::to_string(std::hash<std::string>()(omni_graph.g[vertex_stack.top()].control_flow_str + flow_str));

            omni_graph.g[vertex_stack.top()].control_param_str +=
                std::to_string(std::hash<std::string>()(omni_graph.g[vertex_stack.top()].control_param_str + std::to_string(param_hash)));

            omni_graph.g[vertex_stack.top()].control_flow_hash =
                std::hash<std::string>()(std::to_string(control_flow_hash) + std::to_string(omni_graph.g[vertex_stack.top()].control_flow_hash));
            omni_graph.g[vertex_stack.top()].control_param_hash =
                std::hash<std::string>()(std::to_string(control_param_hash) + std::to_string(omni_graph.g[vertex_stack.top()].control_param_hash));
        }

        omni_graph.local_fields.pop();
        omni_graph.local_prim.pop();
        omni_graph.local_obj.pop();

        return;
    }

    void parse_call_exit(const std::string log, OmniGraph &omni_graph)
    {
        auto cep = _parse_call_exit(log);
        const int thread_id = std::stoi(cep["ThreadId"]);

        auto &call_inst_stack = omni_graph.call_inst_stack[thread_id];
        auto &caller_top = call_inst_stack.top();

        assert(not call_inst_stack.empty());
        if (omni_graph.is_debug)
        {
            const std::string parent_name = olda::split(caller_top["other"], ',')[0];
            const std::string param_name = olda::split(cep["other"], ',')[0];
            assert(parent_name == param_name);
        }

        call_inst_stack.pop();
        return;
    }

} // namespace olda