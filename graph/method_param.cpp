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

    std::map<std::string,std::string> _parse_call_param(const std::string log)
    {
        std::map<std::string, std::string> res = olda::parse_bytecode(log);
        
        return res;
    }

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
    // EventId=290,EventType=CALL_PARAM,ThreadId=0,DataId=233,Value=20,objectType=java.util.HashMap,myObject/Parent:setComplexArrayList,Parent.java:41:4
    void parse_call_param(const std::string log, OmniGraph& omni_graph)
    {
        auto cpp = _parse_call_param(log);

        const int thread = std::stoi(cpp["ThreadId"]);
        
        auto& caller = omni_graph.call_inst_stack[thread];
        
        assert(not caller.empty());
        
        auto& caller_top = caller.top();
        
        const std::string parent_name = olda::split(caller_top["other"],',')[0];
        const std::string param_name = olda::split(cpp["other"],',')[0];
        
        // Some method has no object value
        // e.g., toBoolean
//        assert( caller_top.find("objectType") != caller_top.end());

        assert(parent_name == param_name);
        
        bool isObject = cpp.find("objectType")  != cpp.end();
        /**
         * Note: caller should be object
         * 
         */
        const int parent_object_id = std::stoi(caller_top["Value"]);
        
        std::string param;

        if(isObject)
        {
            const int object_id = std::stoi(cpp["Value"]);
            const std::string object_hash = std::to_string(std::hash<std::string>()(omni_graph.object_order[object_id][-1]));
            param = object_hash;

        } else {
            param = cpp["Value"];
        }
        
        omni_graph.object_order[parent_object_id][-1] += olda::clean_log(log) + param;
        // compress
        omni_graph.object_order[parent_object_id][-1] = std::hash<std::string>()(
            omni_graph.object_order[parent_object_id][-1]);
    }

}