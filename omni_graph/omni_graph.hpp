#pragma once
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <utility>
#include <vector>
#include <numeric>
#include <functional>
#include <filesystem>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/isomorphism.hpp>

namespace olda
{
    struct method_edges
    {
    public:
        int cost;
        std::string edge_hash;
        std::string from;
        std::string to;
    };

    struct method_vertex
    {
    public:
        // method hash is the hash of method
        std::string method_str;
        std::string method_hash;

        // param str has param value, the hash value of object
        std::string param_str;
        std::size_t param_hash = -1;

        // flow hash is the flow of inside of the method
        std::string flow_str;
        std::size_t flow_hash = -1;

        // control flow hash is the method-call-chain of the hash

        std::string control_flow_str = "";
        std::size_t control_flow_hash = 0;

        std::string control_param_str = "";
        std::size_t control_param_hash = 0;

        /*
         * context hash is the flow hash of the parent hash.
         */
        std::size_t context_hash;

        std::string output_format;
        // id is unique number for specific method.
        // Note that som
        int id = 0;
        int edge_cnt = 0;

        std::vector<std::string> param_list;
    };

    using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS,
                                        method_vertex,
                                        method_edges>;

    class FileDatas
    {
    private:
        std::vector<std::string> read_file(const std::string filename);
        std::vector<std::vector<std::string>> typefile_parser(const std::string filepath);
        std::vector<std::vector<std::string>> objectfile_parser(const std::string filepath);
        std::map<int, std::string> stringfile_parser(const std::string filepath);
        std::map<int, std::vector<std::string>> exceptionfile_parser(const std::string filepath);

        std::string log_filename, dirname;

    public:
        std::string type_filename, dataids_filename, object_filename, string_filename, exceptions_filename;

        std::vector<std::vector<std::string>> typefile, objectfile;
        std::map<int, std::string> stringfile;
        std::vector<std::map<std::string, std::string>> dataids;
        std::vector<std::string> omni_log;
        std::map<int, std::vector<std::string>> exceptions;

        FileDatas(std::string _log_name, std::string _dir_name);
        void read_omnilog();
        void read_metafile();
        void read_dataids();
    };

    inline FileDatas::FileDatas(std::string _log_name, std::string _dir_name) : log_filename(_log_name), dirname(_dir_name)
    {
        this->read_omnilog();
        this->read_metafile();
        this->read_dataids();
    };

    struct OmniGraph
    {

        OmniGraph(){};

        ~OmniGraph(){};

        Graph g; // Main Graph
        Graph::vertex_descriptor root;
        // {ThreadId : stack<>}
        std::map<int, std::stack<Graph::vertex_descriptor>> vertex_stack; // call stack

        // {ThreadId : stack<>}
        std::map<int, std::stack<std::map<std::string, std::string>>> caller;

        // {ThreadId : CALL_Instruction of stack<>}
        std::map<int, std::stack<std::map<std::string, std::string>>> call_inst_stack;

        std::string context; // for weak hash
        std::vector<std::string> omni_log;

        // { owner -> FieldName : value }
        std::map<std::string, std::map<std::string, std::string>> static_fields;
        // it must be stack... whenever call method or fuction, store all data
        std::stack<std::map<std::string, std::string>> local_fields;
        // var id to value
        std::stack<std::map<int, std::string>> local_prim;
        // var id to object_id
        std::stack<std::map<int, std::string>> local_obj;

        // the object information for calluculating object hash value.
        // if the object is not array, index should be -1.

        // object_id -> [idx : value]
        std::map<int, std::map<int, std::string>> object_order;

        // for the primitive value.
        // map parent_id to object id. parent_id is recoreded in dataids.
        std::map<int, int> par_to_objId;
        // map parent_id to array_index. this method is used only method realted with array.
        std::map<int, int> array_par_to_idx;

        // parsed dataids
        std::vector<std::map<std::string, std::string>> dataids;
        // object_id -> string
        std::vector<std::vector<std::string>> typefile;
        // value -> object_id
        std::vector<std::vector<std::string>> objectfile;
        // object_id -> value
        std::map<int, std::string> stringfile;
        // id -> exception  & trace
        std::map<int, std::vector<std::string>> exceptions;

        bool is_debug;

        bool firstV = true;

        std::string target_method = "";

        std::vector<Graph::vertex_descriptor> path;
        inline void setFileData(FileDatas fd)
        {
            omni_log = fd.omni_log;
            dataids = fd.dataids;
            typefile = fd.typefile;
            objectfile = fd.objectfile;
            stringfile = fd.stringfile;
        };
    };

    std::vector<std::string> split(const std::string s, char terminator);
    bool is_exist(const std::vector<std::string> v, std::string log);
    bool is_string_type(const std::string &log);
    bool is_primitive_type(const std::string &log);
    std::string extract_method_from_dataids(const std::string event_detail, const std::string elem);
}