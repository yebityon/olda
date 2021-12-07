#pragma once
#include "graph.hpp"

namespace olda
{
    const std::vector<std::string> sp_write_array_key = {};

    std::map<std::string, std::string>
    _parse_write_array(const std::string write_array, const std::vector<std::string> &key = {})
    {
        std::map<std::string, std::string> mp = olda::parse_bytecode(write_array);
        const std::string other = mp["other"];
        std::vector<std::string> tmp = olda::split(other, ',');

        for (int i = 0; i < key.size(); ++i)
        {
            mp[key[i]] = tmp[i];
        }
        return mp;
    }
    /*
    Array order (ARRAY_STORE,ARRAY_STORE_INDEX,ARRAY_STORE_VALUE) DO NOT mention var name.
    It directly affect array object.
    */
    void parse_write_array(const std::string log, OmniGraph &omni_graph)
    {

        std::map<std::string, std::string> wop = _parse_write_array(log);

        if (log.find("VALUE") != std::string::npos)
        {
            // "ARRAY_STORE_VALUE"
            const int dataid = std::stoi(wop["DataId"]);
            const std::string event_detail = omni_graph.dataids[dataid]["EventDetail"];
            const int parent_id = std::stoi(event_detail.substr(event_detail.find("Parent=") + 7));
            const int target_obj_id = omni_graph.par_to_objId[parent_id];
            // par_to_idx has
            const int index = omni_graph.array_par_to_idx[parent_id];

            bool is_object = (log.find("objectType") != std::string::npos);
            if (is_object)
            {
                // Stored value is Object
                const int object_id = std::stoi(wop["Value"]);
                std::string object_value;
                if (olda::is_string_type(wop["objectType"]))
                {
                    object_value = omni_graph.object_order[object_id][-1];
                }
                else
                {
                    object_value = omni_graph.stringfile[object_id];
                }
                omni_graph.object_order[target_obj_id][index] += object_value;
            }
            else
            {
                const std::string value = wop["Value"];
                omni_graph.object_order[target_obj_id][index] += value;
            }
        }
        else if (log.find("INDEX") != std::string::npos)
        {

            const int index = std::stoi(wop["Value"]);
            const int dataid = std::stoi(wop["DataId"]);
            const std::string event_detail = omni_graph.dataids[dataid]["EventDetail"];
            const int parent_id = std::stoi(event_detail.substr(event_detail.find("Parent=") + 7));
            const int target_obj_id = omni_graph.par_to_objId[parent_id];
            omni_graph.array_par_to_idx[parent_id] = index;
        }
        else if (wop["EventType"].find("ARRAY_STORE") != std::string::npos)
        {
            // "ARRAY_STORE"
            // objId :　値が格納される配列のオブジェクトのID
            const int dataid = std::stoi(wop["DataId"]);
            const int objId = std::stoi(wop["Value"]);
            const std::string event_detail = omni_graph.dataids[dataid]["EventDetail"];
            omni_graph.par_to_objId[dataid] = objId;
        }
        else
        {
            // throw exception. something is wrong
            std::cout << "WTF" << std::endl;
            exit(1);
        }
        return;
    }

} //