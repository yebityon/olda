#pragma once
#include "omni_graph.hpp"

namespace olda
{
    const std::set<std::string> primitive{"void", "boolean", "byte", "char", "double", "float", "int", "long", "short", "I", "D"};

    bool is_exist(const std::vector<std::string> v, std::string log)
    {
        for (auto &u : v)
            if (log.find(u) != std::string::npos)
                return true;
        return false;
    };

    std::vector<std::string> split(const std::string s, char terminator)
    {
        std::vector<std::string> v;
        std::string tmp = "";
        for (const auto c : s)
        {
            if (c == terminator)
            {
                v.emplace_back(tmp);
                tmp.clear();
            }
            else
            {
                tmp.push_back(c);
            }
        }
        if (tmp.size() > 0)
        {
            v.emplace_back(tmp);
        }
        return v;
    }

    std::string extract_method_from_dataids(const std::string event_detail, const std::string elem)
    {
        if (elem.find("=") == std::string::npos)
        {
            std::cout << " key must have = character" << std::endl;
            exit(0);
        }
        const int pos = event_detail.find(elem) + elem.length();
        std::string res = "";
        for (size_t i = pos; i < event_detail.size(); ++i)
        {
            res.push_back(event_detail[i]);
        }
        return res;
    }

    bool is_string_type(const std::string &log)
    {
        return log == "string" or log == "java.lang.String";
    }

    bool is_primitive_type(const std::string &log)
    {
        return primitive.find(log) != primitive.end();
    }
}