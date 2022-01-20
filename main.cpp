#include "graph/graph.hpp"
#include "graph_diff/tree_diff.hpp"
#include "writer/graph_writer.hpp"

#define debug_flush(var) std::cout << #var "=" << var << std::endl

#include <iostream>
#include <string>
#include <vector>

#include <chrono>

inline void progress(const std::string message)
{
    std::cout << "[olda]: " << message << std::endl;
}

auto getTime(std::chrono::system_clock::time_point a, std::chrono::system_clock::time_point b)
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(b - a).count();
}

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
        else if (arg == "--param")
        {
            opt["param"] = "valid";
        }
        else if (arg == "--debug")
        {
            opt["debug"] = "valid";
        }
        else if (arg == "--hard")
        {

            opt["hard"] = "valid";
        }
        else if (arg == "--easy")
        {

            opt["easy"] = "easy";
        }
        else if (arg.find("--") != std::string::npos)
        {
            std::cout << "invalid option" << std::endl;
            exit(1);
        }
        else if (arg.find("-") != std::string::npos && arg[0] == '-')
        {
            std::cout << arg << std::endl;
            opt["target"] = arg.substr(1);
        }
        else
        {
            inputfiles.emplace_back(arg);
        }
    }
    std::chrono::system_clock::time_point start, end;

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
    debug_flush(opt["param"]);
    debug_flush(opt["debug"]);
    debug_flush(opt["target"]);

    progress("analyzing metafile....");
    olda::FileDatas origin(origin_log, origin_dir);
    olda::FileDatas target(target_log, target_dir);
    std::cout << "DONE!!\n";

    //===================== ORIGIN GRAPH ======================
    start = std::chrono::system_clock::now();
    progress("constructing origin graph....");
    const auto origin_graph = construct_graph(origin, opt);
    end = std::chrono::system_clock::now();

    auto originGraphTime = getTime(start, end);

    progress("writing origin tree file....");
    olda::write_graphviz(origin_graph, "./target/origin.dot");
    std::cout << "DONE!" << std::endl;

    //===================== ORIGIN GRAPH ======================

    //===================== TARGET GRAPH ======================
    std::cout << "[olda]: constructing target graph....";

    start = std::chrono::system_clock::now();
    const auto target_graph = construct_graph(target, opt);
    end = std::chrono::system_clock::now();
    auto targetGraphTime = getTime(start, end);
    std::cout << " DONE!!\n";

    std::cout << "[olda]: writing target tree file...";
    olda::write_graphviz(target_graph, "./target/target.dot");
    std::cout << " DONE!!\n";

    //===================== TARGET GRAPH ======================

    std::cout << "[olda]: calculating diff_graph....";

    //    const auto graph_diff = olda::diff(origin_graph, target_graph, opt);
    start = std::chrono::system_clock::now();
    const auto graph_diff = (opt["hard"] == "valid" ? olda::diff(origin_graph, target_graph, opt) : olda::easy_diff(origin_graph, target_graph, opt));
    end = std::chrono::system_clock::now();

    auto diffGraphTime = getTime(start, end);

    std::cout << " DONE!!\n";

    std::cout << "[olda]: writing diff tree....";
    olda::write_diffGraph(graph_diff, "./target/diff.dot");
    std::cout << " DONE!!\n";
    std::cout << "[olda]: successfully finish!!" << std::endl;

    end = std::chrono::system_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "[olda]: originGraphBuildTime: " << originGraphTime << "[ms], " << originGraphTime / 1000 << "[s]" << std::endl;
    std::cout << "[olda]: targetGraphBuildTime: " << targetGraphTime << "[ms], " << targetGraphTime / 1000 << "[s]" << std::endl;
    std::cout << "[olda]: diffGraphBuildTime: " << diffGraphTime << "[ms], " << diffGraphTime / 1000 << "[s]" << std::endl;
}