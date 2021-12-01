#pragma once 
#include "../fileparser.hpp"

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
    
namespace olda{


struct  method_edges {
    public:
    int cost;
    std::string edge_hash;
    std::string from;
    std::string to;
};

struct method_vertex {
    public:
    // method hash is the hash of method 
    std::string method_str;
    std::string method_hash;
    // 
    std::string param_str;
    std::size_t param_hash;
    
    std::string flow_str;
    std::size_t flow_hash;
    
    std::string value_str;
    std::size_t value_hash;

    std::string output_format;
    // id is unique number for specific method. 
    // Note that som
    int id = 0;
    int edge_cnt = 0;
    
    std::string weak_flow;
    std::size_t weak_hash;
    
    std::vector<std::string> param_list;
};

     using Graph = boost::adjacency_list<boost::listS,boost::vecS,boost::bidirectionalS,
method_vertex,
method_edges>;

    const std::vector<std::string> entry_orders= {
        "METHOD_ENTRY" // 1
        };
    const std::vector<std::string> exit_orders = {
        "METHOD_NORMAL_EXIT" // greater than  1,
    };
    const std::vector<std::string> method_param = {
        "METHOD_PARAM"
    };
    const std::vector<std::string> read_orders = {
        "GET_INSTANCE_FIELD", "GET_STATIC_FIELD","ARRAY_LOAD","ARRAY_LOAD_INDEX","LOCAL_LOAD"
    };
    const std::vector<std::string> write_orders = {
        "PUT_INSTANCE_FIELD","PUT_INSTANCE_FIELD_VALUE",
        "PUT_STATIC_FIELD", 
        "ARRAY_STORE","ARRAY_SOTRE_VALUE","ARRAY_STORE_INDEX",
        "LOCAL_STORE","LOCAL_INCREMENT"
    };

struct OmniGraph {
    
    OmniGraph(){};
    
    ~OmniGraph(){};
    
    Graph g; //Main Graph 
    Graph::vertex_descriptor root; // the root of Main graph
    std::stack<Graph::vertex_descriptor> vertex_stack; // call stack 
    std::vector<std::string> res; // 
    std::stack<std::map<std::string,std::string>> caller; 
    std::string context;  // for weak hash 
    std::vector<std::string> omni_log;
    
    std::map<std::string,std::vector<std::string>> method_param_list;
    
    
    std::map<std::string, std::map<std::string,std::string>>static_fields;
    // it must be stack... whenever call method or fuction, store all data
    std::stack<std::map<std::string, std::string>>local_fields;
    // var id to value
    std::stack<std::map<int, std::string>>local_prim;
    // var id to object_id
    std::stack<std::map<int, std::string>>local_obj;
    
    // the object information for calluculating object hash value.
    // if the object is not array, index should be -1.

    // object_id -> [idx : value]
    std::map<int, std::map<int,std::string>> object_order;
    
    // for the primitive value.
    std::map<int,std::string>variable;
    
    // map parent_id to object id. parent_id is recoreded in dataids.
    std::map<int,int>par_to_objId;
    // map parent_id to array_index. this method is used only method realted with array.
    std::map<int,int>array_par_to_idx;
    
    std::map<int, std::map<int,std::vector<std::string>>> debug_array_order;
    
    
    
    // parsed dataids 
    std::vector<std::map<std::string,std::string>> dataids;
    // object_id -> string
    std::vector<std::vector<std::string>> typefile;
    // value -> object_id
    std::vector<std::vector<std::string>> objectfile;
    // object_id -> value
    std::map<int,std::string> stringfile;
    void setFileData(FileDatas fd){
        omni_log = fd.omni_log;
        dataids = fd.dataids;
        typefile = fd.typefile;
        objectfile = fd.objectfile;
        stringfile = fd.stringfile;
    };
};
    void parse_method_entry(const std::string log, OmniGraph& g);
    void parse_method_param(const std::string log, OmniGraph& g);
    void parse_method_exit(const std::string log, OmniGraph& g);
    void parse_write_array(const std::string log, OmniGraph& g);
    void parse_write_object(const std::string log, OmniGraph& g);

} // namespace olda
