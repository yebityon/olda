#pragma once
#include "graph.hpp"

namespace olda
{
    // Note: sp_method is the description of the other.
    const std::vector<std::string> sp_method_entry_key = {
        "Line", "LineNum",
        "Class",
        "MethodName", "Type", "Num", "File", "Hash", "MethodFullName", "FileNum"};
    std::map<std::string, std::string> _parse_method_entry(const std::string method_entry)
    {
        std::map<std::string, std::string> mp = olda::parse_bytecode(method_entry);

        const std::string other = mp["other"];
        // try to split mp["other"] infos.
        const std::vector<std::string> tmp = olda::split(other, ',');

        for (int i = 0; i < tmp.size(); ++i)
        {
            mp[sp_method_entry_key[i]] = tmp[i];
        }
        // start from parse
        return mp;
    }

    std::map<std::string, std::string> _parse_call(const std::string log)
    {

        std::map<std::string, std::string> res = olda::parse_bytecode(log);
        // Just ignore "other" information.
        return res;
    }

    void parse_method_entry(const std::string log, OmniGraph &omni_graph)
    {

        omni_graph.local_fields.push(std::map<std::string, std::string>());
        omni_graph.local_prim.push(std::map<int, std::string>());
        omni_graph.local_obj.push(std::map<int, std::string>());

        std::map<std::string, std::string> mep = _parse_method_entry(log);
        const int thread = std::stoi(mep["ThreadId"]);

        auto &caller = omni_graph.caller[thread];
        auto &vertex_stack = omni_graph.vertex_stack[thread];

        if (caller.empty())
        {
            // if the caller is empty, it indicate that this is the initial method.

            omni_graph.root = add_vertex(omni_graph.g);
            omni_graph.g[omni_graph.root].method_hash = mep["Hash"];
            omni_graph.g[omni_graph.root].edge_cnt += 1;
            omni_graph.g[omni_graph.root].method_str = mep["MethodFullName"];
            omni_graph.g[omni_graph.root].flow_str = "";
            caller.push(mep);
            vertex_stack.push(omni_graph.root);
            omni_graph.path.emplace_back(omni_graph.root);

            return;
        }

        auto &prev_method = caller.top();
        Graph::vertex_descriptor from = vertex_stack.top();
        Graph::vertex_descriptor to = add_vertex(omni_graph.g);

        std::string prev_method_name = prev_method["MethodFullName"];
        std::string current_method_name = mep["MethodFullName"];

        if (prev_method_name.find("maven") == std::string::npos &&
            omni_graph.firstV &&
            prev_method_name.find(omni_graph.target_method) != std::string::npos)
        {
            omni_graph.root = from;
            omni_graph.firstV = false;
        }
        // update current information
        omni_graph.g[to].method_hash = mep["Hash"];
        omni_graph.g[to].method_str = current_method_name;
        omni_graph.g[to].flow_str = omni_graph.context;
        // Note : previous method exsit.
        omni_graph.g[to].context_hash = omni_graph.g[from].flow_hash;

        // Init flow str.
        omni_graph.g[to].flow_str = "";

        omni_graph.g[from].edge_cnt += 1;

        // generete edge
        bool isInserted = false;
        Graph::edge_descriptor e;
        boost::tie(e, isInserted) = add_edge(from, to, omni_graph.g);
        omni_graph.g[e].from = prev_method_name;
        omni_graph.g[e].to = current_method_name;
        omni_graph.g[e].cost = omni_graph.g[from].edge_cnt;

        omni_graph.path.emplace_back(to);

        caller.push(mep);
        vertex_stack.push(to);

        return;
    }

    // EventId=289,EventType=CALL,ThreadId=0,DataId=232,Value=5,objectType=java.util.ArrayList,method:CallType=Regular,Instruction=INVOKEVIRTUAL,Owner=java/util/ArrayList,Name=add,Desc=(Ljava/lang/Object;)Z,,myObject/Parent:setComplexArrayList,Parent.java:41:4

    void parse_call_entry(const std::string log, OmniGraph &omni_graph)
    {
        std::map<std::string, std::string> mp = _parse_call(log);

        bool isObject = mp.find("objectType") != mp.end();
        const int thread_id = std::stoi(mp["ThreadId"]);
        auto &call_inst_stack = omni_graph.call_inst_stack[thread_id];

        if (isObject)
        {
            // this call method is for object
            bool isString = is_string_type(mp["objectType"]);
            const int object_id = std::stoi(mp["Value"]);
            omni_graph.object_order[object_id][-1] += olda::clean_log(log);
        }
        else
        {
            // std::cout << "[olda] Warning : Not object CALL is detected." << std::endl;
            // std::cout << "======== " << log << " ========" << std::endl;
        }

        call_inst_stack.push(mp);

        return;
    }

}