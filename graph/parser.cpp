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
    OmniGraph construct_graph(FileDatas &fd)
    {
        OmniGraph omni_graph;
        omni_graph.setFileData(fd);
        for (auto &[object_id, str] : fd.stringfile)
        {
            omni_graph.object_order[object_id][-1] = str;
        }
        int filisize = omni_graph.omni_log.size();
        int crt_progress = 0;
        long long cnt = 0;
        for (std::string log : omni_graph.omni_log)
        {
            cnt += 1;
            if (cnt * 100 / filisize > crt_progress)
            {
                crt_progress = cnt * 100 / filisize;
                std::cout << crt_progress << "%" << std::endl;
            }

            if (is_exist(entry_orders, log))
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
                    std::cout << "EMPTY LOG : " + log << std::endl;
                }
                parse_method_entry(log, omni_graph);
            }
            else if (is_exist(method_param, log))
            {

                parse_method_param(log, omni_graph);
            }
            else if (is_exist(exit_orders, log))
            {

                parse_method_exit(log, omni_graph);
            }
            else if (is_exist(write_orders, log))
            {

                auto tmp = omni_graph.object_order;

                if (log.find("ARRAY") != std::string::npos)
                {
                    parse_write_array(log, omni_graph);
                }
                else
                {
                    parse_write_object(log, omni_graph);
                }
            }
            else if (is_exist(read_orders, log))
            {
            }
            else
            {
                const int thread_id = std::stoi(extract_method_from_dataids(log, "ThreadId="));
                auto &vertex_stack = omni_graph.vertex_stack[thread_id];
                auto &caller = omni_graph.caller[thread_id];

                if (vertex_stack.empty())
                    continue;
                caller.top()["flow_info"] += log;
                omni_graph.g[vertex_stack.top()].flow_str += log;
            }
        }

        std::cout << "=================== DEBUG ======================" << std::endl;

        std::cout << " **************** object_order *****************" << std::endl;
        for (auto &mp : omni_graph.object_order)
        {
            //            std::cout << " ++++++ " << mp.first << " : " << omni_graph.typefile[std::stoi(omni_graph.objectfile[mp.first][1])][1] << " ++++++ " << std::endl;
            for (auto &itr : mp.second)
            {
                std::cout << itr.first << " : " << itr.second << std::endl;
            }
        }
        std::cout << " **************** object_order *****************" << std::endl;

        std::cout << "=================== END  ====================" << std::endl;

        return omni_graph;
    }
}
