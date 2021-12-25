#pragma once
#include "graph.hpp"

namespace olda
{

    const std::vector<std::string> sp_method_param_key = {
        "MethodFullName", "FileNum"};

    std::map<std::string, std::string> _parse_method_param(const std::string method_param)
    {
        std::map<std::string, std::string> mp = olda::parse_bytecode(method_param);
        const std::string other = mp["other"];
        std::vector<std::string> tmp = olda::split(other, ',');

        for (int i = 0; i < tmp.size(); ++i)
        {
            mp[sp_method_param_key[i]] = tmp[i];
        }

        return mp;
    };

    void parse_method_param(const std::string log, OmniGraph &omni_graph)
    {

        auto mpp = _parse_method_param(log);
        const int thread = std::stoi(mpp["ThreadId"]);
        
        auto& caller = omni_graph.caller[thread];
        auto& vertex_stack = omni_graph.vertex_stack[thread];
        

        std::string param;
        std::string caller_vertex = caller.top()["Hash"];

        bool isObject = mpp.find("objectType") != mpp.end();
        if (isObject)
        {
            const int object_id = std::stoi(mpp["Value"]);
            const std::string object_hash = std::to_string(std::hash<std::string>()(omni_graph.object_order[object_id][-1]));
            param = object_hash;
        }
        else
        {
            param = mpp["Value"];
        }
        if(omni_graph.is_debug)
        {
            std::cout << omni_graph.g[vertex_stack.top()].method_str << " -> " << param << std::endl;
            
        }
        omni_graph.g[vertex_stack.top()].param_list.push_back(param);
        return;
    }

}