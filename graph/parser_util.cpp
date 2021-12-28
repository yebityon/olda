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

    std::string clean_log(const std::string log)
    {
        auto parsed_log = olda::split(log, ',');

        // earse line number
        parsed_log.back() = parsed_log.back().substr(0, parsed_log.back().find(':'));

        // erase dataid
        // Value and EventId might be different... because of the order of bytecode.
        auto itr = std::remove_if(parsed_log.begin(), parsed_log.end(),
                                  [](const std::string e)
                                  {
                                      return e.find("DataId=") != std::string::npos ||
                                             e.find("Value=") != std::string::npos ||
                                             e.find("EventId=") != std::string::npos;
                                  });
        parsed_log.erase(itr, parsed_log.end());

        return std::accumulate(parsed_log.begin(), parsed_log.end(), std::string(""));
    }
    
}