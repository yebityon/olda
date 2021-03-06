#pragma once
#include "graph.hpp"

namespace olda
{
    const std::vector<std::string> sp_write_object_key = {};

    std::map<std::string, std::string>
    _parse_write_object(const std::string write_object_log, const std::vector<std::string> &key = {})
    {
        std::map<std::string, std::string> mp = olda::parse_bytecode(write_object_log);
        const std::string other = mp["other"];
        std::vector<std::string> tmp = olda::split(other, ',');

        //        assert(other.size() == key.size());

        for (int i = 0; i < key.size(); ++i)
        {
            mp[key[i]] = tmp[i];
        }
        return mp;
    }

    void parse_write_object(const std::string log, OmniGraph &omni_graph)
    {

        std::map<std::string, std::string> wop = _parse_write_object(log);

        const std::string eventType = wop["EventType"];
        const int dataid = std::stoi(wop["DataId"]);
        const std::string event_detail = omni_graph.dataids[dataid]["EventDetail"];

        // ============== Object =====================
        //    log : EventId=153,EventType=LOCAL_STORE,ThreadId=0,DataId=112,Value=11,objectType=myLibrary.myObject,argtes:main,argtes.java:23:91
        // dataid : 112,0,1,23,91,LOCAL_STORE,Ljava/lang/Object;,Var=11,Name=Gaen,Type=LmyLibrary/myObject;

        // ============== Primitive ====================

        //    log : EventId=26,EventType=LOCAL_STORE,ThreadId=0,DataId=39,Value=123,argtes:main,argtes.java:11:27
        // dataid : 39,0,1,11,27,LOCAL_STORE,I,Var=7,Name=Oikura_Sodachi,Type=I

        if (eventType == "LOCAL_STORE")
        {

            const int var_id = std::stoi(extract_method_from_dataids(event_detail, "Var="));
            const std::string var_name = extract_method_from_dataids(event_detail, "Name=");
            bool isObject = (wop.find("objectType") != wop.end());

            if (isObject)
            {

                const int object_id = std::stoi(wop["Value"]);
                omni_graph.local_obj.top()[var_id] = object_id;
                omni_graph.object_order[object_id][-1] += eventType;

                if (is_string_type(wop["objectType"]))
                {
                    const std::string string_in = omni_graph.stringfile[object_id];
                    omni_graph.object_order[object_id][-1] = omni_graph.stringfile[object_id];
                }
                else /*  the object is not string type */
                {
                    // Nothing to recored.
                }
            }
            else /* the variable is primitive */
            {
                const std::string prim_value = wop["Value"];
                omni_graph.local_prim.top()[var_id] = prim_value;
            }
        }
        else if (eventType == "LOCAL_INCREMENT")
        {

            // this order will not be called.
            const int var_id = std::stoi(extract_method_from_dataids(event_detail, "Var="));
            const std::string increment = extract_method_from_dataids(event_detail, "Increment=");

            if (log.find("objectType") != std::string::npos)
            {

                const int object_id = std::stoi(wop["Value"]);
                omni_graph.object_order[object_id][-1] += eventType;
                omni_graph.object_order[object_id][-1] += increment;
            }
        }
        else
        {
            // EvnetType should be PUT_INSTANCE_FIELD , PUT_INSTANCE_FIELD_VALUE
            // PUT_INSTANCE_FIELD_BEFORE_INITIALIZATION , PUT_STATIC_FIELD
            if (eventType == "PUT_INSTANCE_FIELD")
            {

                const std::string owner = extract_method_from_dataids(event_detail, "Owner=");
                const std::string fieldName = extract_method_from_dataids(event_detail, "Fieldname=");
                const std::string fieldType = extract_method_from_dataids(event_detail, "Type=");
                const int dataid = std::stoi(wop["DataId"]);

                // const std::string method_method_used_to_method?
                const int target_obj = std::stoi(wop["Value"]);
                // target_obj has instance, it indicate that target obj should be object.
                assert(!is_primitive_type(omni_graph.typefile[std::stoi(omni_graph.objectfile[target_obj][1])][1]));

                omni_graph.par_to_objId[dataid] = target_obj;
                omni_graph.object_order[target_obj][-1] += eventType;
            }
            else if (eventType == "PUT_INSTANCE_FIELD_VALUE")
            {

                const int par_id = std::stoi(extract_method_from_dataids(event_detail, "Parent="));
                const int target_obj = omni_graph.par_to_objId[par_id];
                const std::string store_value = wop["Value"];

                if (wop["Value"].size() > 9)
                {
                    std::cout << "Warning: Invalid value size:" << log << ":" << wop["Value"] << std::endl;
                    return;
                }

                const std::string store_value_type = extract_method_from_dataids(event_detail, "Type=");

                if (is_primitive_type(store_value_type))
                {
                    // assert( std::cout << "primitive INSTANCE_FILED is not defined\n" && false );
                }
                else /* the sotre_value_type is object */
                {

                    const int store_obj_idx = std::stoi(wop["Value"]);

                    if (is_string_type(wop["objectType"]))
                    {

                        const std::string obj_string = omni_graph.stringfile[store_obj_idx];
                        omni_graph.object_order[target_obj][-1] += obj_string;
                    }
                    else
                    {
                        const std::string obj_hash = std::to_string(std::hash<std::string>()(omni_graph.object_order[store_obj_idx][-1]));
                        omni_graph.object_order[target_obj][-1] += obj_hash;
                    }
                }
            }
            else if (eventType == "PUT_INSTANCE_FIELD_BEFORE_INITIALIZATION")
            {
                //
                //
                // Nothing to Do
            }
            else if (eventType == "PUT_STATIC_FIELD")
            {
                // PUT_STATIC_FIELD is the instruction when static FIELD is stored.
                // Therefore, It should be first instruction of these field. then,sotre this instruction to omni_graph and static_filed

                // EventId=3,EventType=PUT_STATIC_FIELD,ThreadId=0,DataId=337,Value=1192,argtes:<clinit>,argtes.java:8:3
                // EventId = 4, EventType = PUT_STATIC_FIELD, ThreadId = 0, DataId = 338, Value = 1610, argtes : <clinit>, argtes.java : 8 : 5

                bool isObject = (wop.find("objectType") == wop.end());
                const std::string owner = extract_method_from_dataids(event_detail, "Owner=");
                const std::string fieldName = extract_method_from_dataids(event_detail, "FieldName=");

                if (isObject)
                {
                    // TODO : cehck the array.

                    if (is_string_type(wop["ojbectType"]))
                    {
                        const int object_id = std::stoi(wop["Value"]);
                        const std::string str_value = omni_graph.stringfile[object_id];
                        omni_graph.object_order[object_id][-1] += eventType;

                        omni_graph.static_fields[owner][fieldName] += str_value;
                    }
                    else
                    {
                        if (wop["Value"].size() > 9)
                        {
                            std::cout << "Warning: Invalid value size:" << log << ":" << wop["Value"] << std::endl;
                            return;
                        }
                        const int object_id = std::stoi(wop["Value"]);
                        omni_graph.object_order[object_id][-1] += eventType;

                        const std::string order = omni_graph.object_order[object_id][-1];
                        omni_graph.static_fields[owner][fieldName] += order;
                    }
                }
                else
                {
                    const std::string prim_value = wop["Value"];
                    omni_graph.static_fields[owner][fieldName] += prim_value;
                }
            }
        }
    }
}