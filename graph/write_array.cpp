#pragma once
#include "graph.hpp"

namespace olda
{

    const std::vector<std::string> write_array_key = {};

    std::map<std::string, std::string> _parse_write_array(const std::string write_object_log)
    {

        std::vector<std::string> parsed_log = olda::split(write_object_log, ',');

        std::map<std::string, std::string> mp;
        return mp;

        // for(auto& key : basic_key){
        //     // show that exist
        //     this -> keys[key] = 1;
        // }
        // for(auto& parsed_inst : parsed_log) {
        //     std::vector<std::string> inst_info = olda : split(parsed_inst,'=');
        //     if(inst_info.size()  <  2) {
        //         // No "=" in the log
        //         this -> info["other"] += " " + inst_info.front();
        //     } else {
        //         this -> info[inst_info[0]] = inst_info[1];
        //     }
        // }
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
                if (wop["objectType"] != "java.lang.String")
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
        }
        return;
    }

} //