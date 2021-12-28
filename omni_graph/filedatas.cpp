#include "omni_graph.hpp"

namespace olda
{
    std::vector<std::string> FileDatas::read_file(const std::string filename)
    {
        std::ifstream file_stream;
        std::vector<std::string> res;

        file_stream.open(filename, std::ios::in);
        while (not file_stream.eof())
        {
            std::string buffer;
            std::getline(file_stream, buffer);
            if (buffer.size() == 0)
            {
                // seems like empty file
                continue;
            }
            res.emplace_back(buffer);
        }
        return res;
    }

    std::vector<std::vector<std::string>> FileDatas::typefile_parser(const std::string filepath)
    {
        auto vs = read_file(filepath);
        const int N = vs.size();
        std::vector<std::vector<std::string>> res(N);
        for (int i = 0; i < N; ++i)
        {
            std::string s = vs[i];
            auto v = olda::split(s, ',');
            res[i] = v;
        }
        return res;
    };

    std::vector<std::vector<std::string>> FileDatas::objectfile_parser(const std::string filepath)
    {
        auto vs = read_file(filepath);
        const int N = vs.size();
        std::vector<std::vector<std::string>> res(N + 1, std::vector<std::string>());
        res.front().push_back("INVALID_TYPE");
        for (int i = 0; i < N; ++i)
        {
            std::vector<std::string> s = olda::split(vs[i], ',');
            res[i + 1] = s;
        }
        return res;
    };
    // only for String object
    std::map<int, std::string> FileDatas::stringfile_parser(const std::string filepath)
    {
        auto vs = read_file(filepath);
        std::map<int, std::string> res;
        for (const auto &s : vs)
        {
            const auto vs = olda::split(s, ',');
            const int id = std::stoi(vs.front());
            res[id] = vs[2];
        }
        return res;
    };
    
    std::map<int,std::vector<std::string>> FileDatas::exceptionfile_parser(const std::string filepath)
    {
        auto vs = read_file(filepath);
        std::map<int, std::vector<std::string>> res;
        for(const auto& s : vs){
            std::cout << s << std::endl;
            auto parsed = olda::split(s, ',');
            const int id = std::stoi(parsed.front());
            parsed.erase(parsed.begin());
            res[id] = parsed;
        }
        return res;
    }

    void FileDatas::read_metafile()
    {
        for (const std::filesystem::directory_entry &i : std::filesystem::directory_iterator(this->dirname))
        {
            auto path = i.path();
            if (path.filename() == "dataids.txt")
            {
                this->dataids_filename = path.string();
            }
            else if (path.filename() == "LOG$Types.txt")
            {
                this->type_filename = path.string();
            }
            else if (path.string().find("LOG$Object") != std::string::npos)
            {
                this->object_filename = path.string();
            }
            else if (path.string().find("LOG$String00001") != std::string::npos)
            {
                this->string_filename = path.string();
            }
            else if(path.string().find("LOG$Exceptions") != std::string::npos){
               this-> exceptions_filename = path.string();
            }
        }
        this->typefile = this->typefile_parser(this->type_filename);
        this->objectfile = this->objectfile_parser(this->object_filename);
        this->stringfile = this->stringfile_parser(this->string_filename);
//        this->exceptions = this->exceptionfile_parser(this->exceptions_filename);

        return;
    }

    void FileDatas::read_dataids()
    {
        std::vector<std::map<std::string, std::string>> res;
        std::vector<std::string> dataids_vector = this->read_file(this->dataids_filename);
        for (const auto &dataid : dataids_vector)
        {
            // for future extension
            int keytype = 0;
            std::map<std::string, std::string> elem;
            std::string tmp;
            for (auto c : dataid)
            {
                if (c == ',')
                {
                    if (keytype == 0)
                    {
                        elem["DataId"] = tmp;
                    }
                    else if (keytype == 1)
                    {
                        elem["ClassId"] = tmp;
                    }
                    else if (keytype == 2)
                    {
                        elem["MethodId"] = tmp;
                    }
                    else if (keytype == 3)
                    {
                        elem["LineNum"] = tmp;
                    }
                    else if (keytype == 4)
                    {
                        elem["BytecodeNumv"] = tmp;
                    }
                    else if (keytype == 5)
                    {
                        elem["Event"] = tmp;
                    }
                    else
                    {
                        elem["EventDetail"] += tmp;
                        elem["EventDetail"] += ",";
                    }
                    std::string s;
                    tmp.swap(s);
                    keytype += 1;
                }
                else
                {
                    tmp.push_back(c);
                }
            }
            elem["EventDetail"] += tmp;
            res.emplace_back(elem);
        }
        std::sort(res.begin(), res.end(), [](auto& lhs, auto& rhs)
                  { return std::stoi(lhs["DataId"]) < std::stoi(rhs["DataId"]); });
        this->dataids = res;
    }

    void FileDatas::read_omnilog()
    {

        this->omni_log = this->read_file(this->log_filename);
    }
}