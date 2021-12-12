#include "graph/graph.hpp"
#include "graph_diff/tree_diff.hpp"
#include "writer/graph_writer.hpp"

#define debug_flush(var) std::cout << #var "=" << var << std::endl

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
            opt["flow"] = "valid";
        }
        else if (arg == "--value")
        {
            opt["value"] = "valid";
        }
        else if (arg.find("--") != std::string::npos)
        {
            std::cout << "invalid option" << std::endl;
            exit(1);
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

    origin_log = inputfiles[1];
    /* log dir contain classes.txt, dataids.txt*/
    origin_dir = inputfiles[2];
    target_log = inputfiles[3];
    target_dir = inputfiles[4];

    // debug

    debug_flush(origin_log);
    debug_flush(origin_dir);
    debug_flush(target_log);
    debug_flush(target_dir);

    debug_flush(opt["flow"]);
    debug_flush(opt["value"]);

    printf("%s", "[olda]: analyzing metafile....");
    olda::FileDatas origin(origin_log, origin_dir);
    olda::FileDatas target(target_log, target_dir);
    std::cout << "DONE!!\n";

    //===================== ORIGIN GRAPH ======================
    std::cout << "[olda]: constructing origin graph....";
    const auto origin_graph = construct_graph(origin);
    std::cout << "DONE!" << std::endl;

    std::cout << "[olda]: writing origin tree file....";
    olda::write_graphviz(origin_graph, "./target/origin.dot");
    std::cout << "DONE!" << std::endl;

    //===================== ORIGIN GRAPH ======================

    

    //===================== TARGET GRAPH ======================
    std::cout << "[olda]: constructing target graph....";
    const auto target_graph = construct_graph(target);
    std::cout << " DONE!!\n";

    std::cout << "[olda]: writing target tree file...";
    olda::write_graphviz(target_graph, "./target/target.dot");
    std::cout << " DONE!!\n";

    //===================== TARGET GRAPH ======================

    std::cout << "[olda]: calculating diff_graph....";
    
//    const auto graph_diff = olda::diff(origin_graph, target_graph, opt);
    const auto graph_diff = olda::easy_diff(origin_graph, target_graph, opt);
    std::cout << " DONE!!\n";
    
    std::cout << "[olda]: writing diff tree....";
    olda::write_diffGraph(graph_diff, "./target/diff.dot");
    std::cout << " DONE!!\n";
    std::cout << "[olda]: success fully finish!!" << std::endl;
}