
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

class FileDatas{
    
    private:
    std::vector<std::string> read_file(const std::string filename);
    std::vector<std::string> split(const std::string s, char terminator);
    std::vector<std::vector<std::string>> typefile_parser(const std::string filepath);
        std::vector<std::vector<std::string>> objectfile_parser(const std::string filepath);
    std::map<int,std::string> stringfile_parser(const std::string filepath);
    std::string log_filename,dirname;
    public:

    std::string type_filename, dataids_filename, object_filename, string_filename;
    
    std::vector<std::vector<std::string>> typefile,objectfile;
    std::map<int,std::string> stringfile;
    std::vector<std::map<std::string, std::string>> dataids;
    std::vector<std::string> omni_log;
    
    FileDatas(std::string _log_name, std::string _dir_name);
    
    void read_omnilog();
    void read_metafile();
    void read_dataids();
};
// constuctor 
inline FileDatas::FileDatas(std::string _log_name, std::string _dir_name) : log_filename(_log_name), dirname(_dir_name){
    this -> read_omnilog(); 
    this -> read_metafile();
    this -> read_dataids();
}

std::vector<std::vector<std::string>> typefile_parser(const std::string filepath);
std::vector<std::vector<std::string>> objectfile_parser(const std::string filepath);

// only for String object 
std::map<int,std::string> stringfile_parser(const std::string filepath);