#include "../graph/graph.hpp"

const std::vector<std::string> sp_method_param_key = {
    "MethodFullName", "FileNum"};

using str = std::string;

void f(str funcname)
{
    std::cout << funcname << ": done" << std::endl;
    return;
}
std::map<std::string, std::string> _parse_method_param(const std::string method_param)
{
    std::map<std::string, std::string> mp = olda::parse_bytecode(method_param);
    const std::string other = mp["other"];
    std::vector<std::string> tmp = olda::split(other, ',');

    for (int i = 0; i < tmp.size(); ++i)
    {
        mp[sp_method_param_key[i]] = tmp[i];
    }

    return mp;
};

void test_1()
{
    const str in = "EventId=47,EventType=METHOD_PARAM,ThreadId=0,DataId=197,Value=2,myLibrary/myMath:multiple,myMath.java:0:0";
    auto mp = _parse_method_param(in);
    assert(mp["EventId"] == "47");
    assert(mp["EventType"] == "METHOD_PARAM");
    assert(mp["ThreadId"] == "0");
    assert(mp["DataId"] == "197");
    assert(mp["Value"] == "2");
    assert(mp["MethodFullName"] == "myLibrary/myMath:multiple");
    assert(mp["FileNum"] == "myMath.java:0:0");

    f("METHOD_PARAM_1");

    const str in_2 = "EventId=177,EventType=METHOD_PARAM,ThreadId=0,DataId=225,Value=5,objectType=int[],myLibrary/myMath:apply,myMath.java:0:0";

    mp = _parse_method_param(in_2);

    assert(mp["EventId"] == "177");
    assert(mp["EventType"] == "METHOD_PARAM");
    assert(mp["ThreadId"] == "0");
    assert(mp["DataId"] == "225");
    assert(mp["Value"] == "5");
    assert(mp["objectType"] == "int[]");
    assert(mp["MethodFullName"] == "myLibrary/myMath:apply");
    assert(mp["FileNum"] == "myMath.java:0:0");

    f("METHOD_PARAM_2");
}

void test()
{
    test_1();
}
int main()
{
    test();
}