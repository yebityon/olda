#include "../graph/graph.hpp"

const std::vector<std::string> sp_method_entry_key = {
    "Line", "LineNum",
    "Class",
    "MethodName", "Type", "Num", "File", "Hash", "MethodFullName", "FileNum"};

using str = std::string;

void f(str funcname)
{
    std::cout << funcname << ": done" << std::endl;
    return;
}

std::map<std::string, std::string> _parse_method_entry(const std::string method_entry)
{
    std::map<std::string, std::string> mp = olda::parse_bytecode(method_entry);
    const std::string other = mp["other"];
    std::cout << other << std::endl;
    std::vector<std::string> tmp = olda::split(other, ',');

    for (int i = 0; i < tmp.size(); ++i)
    {
        mp[sp_method_entry_key[i]] = tmp[i];
    }
    // start from parse
    return mp;
}
void test_1()
{
    const str in = "EventId=74,EventType=METHOD_ENTRY,ThreadId=0,DataId=464,Value=0,method:2,15,myLibrary/myMath,addInt,(II)I,9,myMath.java,8e0194cf7a9d0d8444202a95246cee9aa368f660,myLibrary/myMath:addInt,myMath.java:0:0";
    auto mp = _parse_method_entry(in);
    assert(mp["EventId"] == "74");
    assert(mp["EventType"] == "METHOD_ENTRY");
    assert(mp["ThreadId"] == "0");
    assert(mp["DataId"] == "464");
    assert(mp["Value"] == "0");
    assert(mp["Line"] == "method:2");
    assert(mp["LineNum"] == "15");
    assert(mp["Class"] == "myLibrary/myMath");
    assert(mp["MethodName"] == "addInt");
    assert(mp["Type"] == "(II)I");
    assert(mp["Num"] == "9");
    assert(mp["File"] == "myMath.java");
    assert(mp["Hash"] == "8e0194cf7a9d0d8444202a95246cee9aa368f660");
    assert(mp["MethodFullName"] == "myLibrary/myMath:addInt");
    assert(mp["FileNum"] == "myMath.java:0:0");
    f("METHOD_ENTRY_1");

    const str in_2 = "EventId=0,EventType=METHOD_ENTRY,ThreadId=0,DataId=13,Value=0,method:0,1,Main,main,([Ljava/lang/String;)V,9,Main.java,fe1fe704c569eaf77bb10120a2a4698035803860,Main:main,Main.java:0:0";

    mp = _parse_method_entry(in_2);

    assert(mp["EventId"] == "0");
    assert(mp["EventType"] == "METHOD_ENTRY");
    assert(mp["ThreadId"] == "0");
    assert(mp["DataId"] == "13");
    assert(mp["Value"] == "0");
    assert(mp["Line"] == "method:0");
    assert(mp["LineNum"] == "1");
    assert(mp["Class"] == "Main");
    assert(mp["MethodName"] == "main");
    assert(mp["Type"] == "([Ljava/lang/String;)V");
    assert(mp["Num"] == "9");
    assert(mp["File"] == "Main.java");
    assert(mp["Hash"] == "fe1fe704c569eaf77bb10120a2a4698035803860");
    assert(mp["MethodFullName"] == "Main:main");
    assert(mp["FileNum"] == "Main.java:0:0");
    f("METHOD_ENTRY_2");
}

void test()
{
    test_1();
}
int main()
{
    test();
}