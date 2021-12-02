#pragma once
#include "graph.hpp"

namespace olda
{

    const std::vector<std::string> method_param_keys = {};

    std::map<std::string, std::string> _parse_method_param(const std::string log)
    {

        std::vector<std::string> key_string;
        auto tokens = split(log, ',');
        auto keys = method_param_keys;

        if (log.find("objectType=") == std::string::npos)
        {
            // erase "objectType" from keys because param is not boject
            keys.erase(keys.begin() + 5);
        }
        if (tokens.size() != keys.size())
        {
            std::cout << log << std::endl;
            std::cout << tokens.size() << std::endl;
            std::cout << keys.size() << std::endl;
            assert(false);
        }
        assert(tokens.size() == keys.size());

        std::map<std::string, std::string> mp;
        for (int i = 0; i < tokens.size(); ++i)
        {
            mp[keys[i]] = tokens[i].substr(keys[i].size() + 1);
        }

        return mp;
    };

    void parse_method_param(const std::string log, OmniGraph &omni_graph)
    {

        auto mpp = _parse_method_param(log);
        std::string param;
        std::string caller_vertex = omni_graph.caller.top()["hash"];

        bool isObject = mpp.find("ObjectType") != mpp.end();

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
        std::cout << omni_graph.caller.top()["method_fullname"] << " " << param << std::endl;

        omni_graph.g[omni_graph.vertex_stack.top()].param_list.push_back(param);
        return;
    }

}