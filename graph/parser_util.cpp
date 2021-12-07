#include "graph.hpp"

namespace olda
{
    std::map<std::string, std::string> parse_bytecode(const std::string inst)
    {
        std::map<std::string, std::string> res;
        std::vector<std::string> parsed_log = olda::split(inst, ',');

        for (auto& inst : parsed_log)
        {
            // for each instruction, get detail
            std::vector<std::string> inst_detail = olda::split(inst, '=');
            if(inst_detail.empty()) continue;

            if (inst_detail.size() < 2)
            {
                // No "=" in the log
                res["other"] += inst_detail.front() + ",";
            }
            else
            {
                res[inst_detail[0]] = inst_detail[1];
            }
        }

        return res;
    }
    
}