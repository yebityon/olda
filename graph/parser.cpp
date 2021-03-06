#include "graph.hpp"

namespace olda
{

    /*
        omni log paerser. Analyzed the SeLogger omni mode log　　　　　　　　　　.
        the graph edge show the method call/exit, vetex show the flow and value difference.
        vetex has the hash value of flow and value difference.
        Strong Compatibility -
        Weak Compatiblitiy - some method has different hash value from different other method.

    */
    OmniGraph construct_graph(FileDatas &fd, std::map<std::string, std::string> &opt)
    {
        OmniGraph omni_graph;
        omni_graph.setFileData(fd);
        omni_graph.target_method = opt["target"];

        for (auto &[object_id, str] : fd.stringfile)
        {
            omni_graph.object_order[object_id][-1] = str;
        }

        int filisize = omni_graph.omni_log.size();
        int crt_progress = 0;
        long long cnt = 0;

        omni_graph.is_debug = (opt["debug"] == "valid");

        for (std::string log : omni_graph.omni_log)
        {
            if (omni_graph.is_debug)
            {
                std::cout << log << std::endl;
            }
            cnt += 1;
            if (cnt * 100 / filisize > crt_progress)
            {
                crt_progress = cnt * 100 / filisize;
                std::cout << crt_progress << "%" << std::endl;
            }

            const std::string eventType = olda::parse_bytecode(log)["EventType"];

            if (eventType == "METHOD_ENTRY" || eventType == "METHOD_NORMAL_EXIT" || eventType == "METHOD_EXCEPTIONAL_EXIT")
            {
                if (log.find("Optimizer:doOptimize") != std::string::npos || log.find("Optimizer:optimize") != std::string::npos)
                {
                    std::cout << log << std::endl;
                }
            }

            if (eventType == "METHOD_ENTRY")
            {
                const int thread_id = std::stoi(extract_method_from_dataids(log, "ThreadId="));
                auto &vertex_stack = omni_graph.vertex_stack[thread_id];
                auto &caller = omni_graph.caller[thread_id];

                // std::string context holds the bytecode of caller method.
                std::string context;

                if (not vertex_stack.empty())
                {
                    context = omni_graph.g[vertex_stack.top()].flow_str;
                }
                else
                {
                    if (omni_graph.is_debug)
                    {
                        std::cout << "EMPTY LOG : " + log << std::endl;
                    }
                }
                parse_method_entry(log, omni_graph);
            }
            else if (eventType == "METHOD_PARAM")
            {
                parse_method_param(log, omni_graph);
            }
            else if (eventType == "METHOD_NORMAL_EXIT" || eventType == "METHOD_EXCEPTIONAL_EXIT")
            {
                // Note : METHOD_exception is also write order.
                parse_method_exit(log, omni_graph);
            }
            else if (eventType == "LOCAL_STORE" ||
                     eventType == "LOCAL_INCREMENT" ||
                     eventType == "PUT_INSTANCE_FIELD" ||
                     eventType == "PUT_INSTANCE_FIELD_VALUE" ||
                     eventType == "PUT_STATIC_FIELD")
            {
                parse_write_object(log, omni_graph);
            }
            else if (eventType == "ARRAY_STORE" || eventType == "ARRAY_STORE_VALUE" ||
                     eventType == "ARRAY_STORE_INDEX")
            {
                parse_write_array(log, omni_graph);
            }
            else if (eventType.find("CALL") != std::string::npos)
            {
                // TODO : CALL Instruction should be recored to the object page
                // ONLY catch CALL Instruction.
                // if (eventType == "CALL")
                // {
                //     parse_call_entry(log, omni_graph);
                // }
                // else if (eventType == "CALL_PARAM")
                // {
                //     parse_call_param(log, omni_graph);
                // }
                // else if (eventType == "CALL_RETURN")
                // {
                //     parse_call_exit(log, omni_graph);
                // }

                const int thread_id = std::stoi(extract_method_from_dataids(log, "ThreadId="));
                auto &vertex_stack = omni_graph.vertex_stack[thread_id];
                auto &caller = omni_graph.caller[thread_id];
                if (vertex_stack.empty())
                    continue;

                const std::string flow_str = olda::clean_log(log);
                omni_graph.g[vertex_stack.top()].flow_str += flow_str;
            }
            else
            {

                const int thread_id = std::stoi(extract_method_from_dataids(log, "ThreadId="));
                auto &vertex_stack = omni_graph.vertex_stack[thread_id];
                auto &caller = omni_graph.caller[thread_id];

                if (vertex_stack.empty())
                    continue;

                const std::string flow_str = olda::clean_log(log);
                omni_graph.g[vertex_stack.top()].flow_str += flow_str;
            }
        }
        if (omni_graph.is_debug)
        {

            std::cout << " **************** object_order *****************" << std::endl;
            for (auto &mp : omni_graph.object_order)
            {
                std::cout << "+++++++++++ " << mp.first << " +++++++++++" << std::endl;

                for (auto &itr : mp.second)
                {
                    std::cout << itr.first << " : " << itr.second << std::endl;
                }
            }
            std::cout << " **************** object_order *****************" << std::endl;

            std::cout << "=================== END  ====================" << std::endl;
        }

        return omni_graph;
    }
}
