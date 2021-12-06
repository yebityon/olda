#include "../graph/graph.hpp"

using str = std::string;

void f(str funcname)
{
    std::cout << funcname << ": done" << std::endl;
    return;
}

const std::vector<std::string> sp_method_exit_key = {
    "MethodFullName", "FileNum"};

std::map<std::string, std::string> _parse_method_exit(const std::string method_exit)
{
    std::map<std::string, std::string> mp = olda::parse_bytecode(method_exit);

    const std::string other = mp["other"];
    const std::vector<std::string> tmp = olda::split(other, ',');

    for (int i = 0; i < tmp.size(); ++i)
    {
        mp[sp_method_exit_key[i]] = tmp[i];
    }

    return mp;
};
void test_1()
{
    const str in = "EventId=360,EventType=METHOD_NORMAL_EXIT,ThreadId=0,DataId=252,Value=5,objectType=int[],myLibrary/myMath:apply,myMath.java:35:28";
    auto mp = _parse_method_exit(in);
    assert(mp["EventId"] == "360");
    assert(mp["EventType"] == "METHOD_NORMAL_EXIT");
    assert(mp["ThreadId"] == "0");
    assert(mp["DataId"] == "252");
    assert(mp["Value"] == "5");
    assert(mp["objectType"] == "int[]");
    assert(mp["MethodFullName"] == "myLibrary/myMath:apply");
    assert(mp["FileNum"] == "myMath.java:35:28");
    f("METHOD_NORMAL_EXIT_1");

    const str in_2 = "EventId=485,EventType=METHOD_NORMAL_EXIT,ThreadId=0,DataId=284,Value=65536,myLibrary/myMath:_multiple,myMath.java:42:27";

    mp = _parse_method_exit(in_2);

    assert(mp["EventId"] == "485");
    assert(mp["EventType"] == "METHOD_NORMAL_EXIT");
    assert(mp["ThreadId"] == "0");
    assert(mp["DataId"] == "284");
    assert(mp["Value"] == "65536");
    assert(mp["MethodFullName"] == "myLibrary/myMath:_multiple");
    assert(mp["FileNum"] == "myMath.java:42:27");
    f("METHOD_NORMAL_EXIT_2");
}

void test()
{
    test_1();
}
int main()
{
    test();
}