#pragma once
#include "../omni_graph/omni_graph.hpp"

namespace olda
{
    const std::vector<std::string> entry_orders = {
        "METHOD_ENTRY" // 1
    };
    const std::vector<std::string> exit_orders = {
        "METHOD_NORMAL_EXIT" // greater than  1,
    };
    const std::vector<std::string> method_param = {
        "METHOD_PARAM"};
    const std::vector<std::string> read_orders = {
        "GET_INSTANCE_FIELD", "GET_STATIC_FIELD", "ARRAY_LOAD", "ARRAY_LOAD_INDEX", "LOCAL_LOAD"};
    const std::vector<std::string> write_orders = {
        "PUT_INSTANCE_FIELD", "PUT_INSTANCE_FIELD_VALUE",
        "PUT_STATIC_FIELD",
        "ARRAY_STORE", "ARRAY_SOTRE_VALUE", "ARRAY_STORE_INDEX",
        "LOCAL_STORE", "LOCAL_INCREMENT"};

    olda::OmniGraph construct_graph(olda::FileDatas &d);

    void parse_method_entry(const std::string log, OmniGraph &g);
    void parse_method_param(const std::string log, OmniGraph &g);
    void parse_method_exit(const std::string log, OmniGraph &g);
    void parse_write_array(const std::string log, OmniGraph &g);
    void parse_write_object(const std::string log, OmniGraph &g);

    std::map<std::string, std::string> parse_bytecode(const std::string inst);

} // namespace olda
