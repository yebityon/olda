#include "graph/graph.hpp"
#include "graph_diff/tree_diff.hpp"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[])
{

    if (argc < 5)
    {

        std::cout << "USAGE : A_log.txt A_dir B_log.txt B_dir" << std::endl;
        exit(0);
    }

    std::vector<std::string> inputfiles;
    std::map<std::string, std::string> opt;

    for (int i = 0; i < argc; ++i)
    {

        const std::string arg = argv[i];

        if (arg == "--flow")
        {
            // weak hash
        }
        else if (arg == "--value")
        {
            // value hash
        }
        else
        {
            inputfiles.emplace_back(arg);
        }
    }

    std::string origin_log, origin_dir;
    std::string origin_dataids, origin_logType, origin_logObject, origin_StringObject;
    std::string target_log, target_dir;
    std::string target_dataids, target_logType, target_logObject, target_StringObject;

    origin_log = argv[1];
    /* log dir contain classes.txt, dataids.txt*/
    origin_dir = argv[2];
    target_log = argv[3];
    target_dir = argv[4];

    olda::FileDatas origin(origin_log, origin_dir);
    olda::FileDatas target(target_log, target_dir);

    const auto origin_graph = construct_graph(origin);
    const auto target_graph = construct_graph(target);

    const auto graph_diff = olda::diff(origin_graph, target_graph,opt);

    // write part.
}