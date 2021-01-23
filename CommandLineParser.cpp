#include "CommandLineParser.h"
#include <iostream>
#include <algorithm>
using namespace std;

std::map<std::string, CommandLineFlagInfo> g_commandLineValueMap;
std::map<std::string, std::string> g_commandLineDescriptionMap;

DEFINE_string(t, "default", "a test command");
DEFINE_string(p, "default", "a test command");
bool CheckNumberOrAlphabet(char ch)
{
    if ((ch >= '0' && ch <= '9') || \
        (ch >= 'a' && ch <= 'z') || \
        (ch >= 'A' && ch <= 'Z') || \
        ch == '_') {
        return true;
    }
    return false;
}

bool ParseCommandLineFlags(int argc, char* argv[])
{
    CommandLineParserAutoMata status = CommandLineParserAutoMata::STATUS0;
    string command;
    string value;
    for (int i = 1; i < argc;) {
        int j = 0;
        while (argv[i][j]) {
            if (status == CommandLineParserAutoMata::STATUS0) {
                if (argv[i][j] != '-') {
                    cout << "parser command line error, line:" << __LINE__ <<endl;
                    return false;
                } else {
                    ++j;
                    status = CommandLineParserAutoMata::STATUS1;
                    continue;
                }
            } else if (status == CommandLineParserAutoMata::STATUS1) {
                if (argv[i][j] == '-') {
                    ++j;
                    status = CommandLineParserAutoMata::STATUS1;
                    continue;
                } else if (CheckNumberOrAlphabet(argv[i][j])) {
                    status = CommandLineParserAutoMata::STATUS2;
                    command += argv[i][j];
                    ++j;
                    continue;
                } else {
                    cout << "parser command line error, line:" << __LINE__ <<endl;
                    return false;
                }
            } else if (status == CommandLineParserAutoMata::STATUS2) {
                if (CheckNumberOrAlphabet(argv[i][j])) {
                    status = CommandLineParserAutoMata::STATUS2;
                    command += argv[i][j];
                    ++j;
                    continue;
                } else if (argv[i][j] == '=') {
                    status = CommandLineParserAutoMata::STATUS3;
                    ++j;
                    continue;
                } else {
                    cout << "parser command line error, line:" << __LINE__ <<endl;
                    return false; 
                }
            } else if (status == CommandLineParserAutoMata::STATUS3) {
                value += argv[i][j];
                ++j;
                status = CommandLineParserAutoMata::STATUS4;
            } else if (status == CommandLineParserAutoMata::STATUS4) {
                value += argv[i][j];
                ++j;
                status = CommandLineParserAutoMata::STATUS4;
            }
        }
        if (command == "help") {
            for_each(g_commandLineDescriptionMap.begin(), g_commandLineDescriptionMap.end(), \
                [](pair<string, string> p) {cout << " --" << p.first << "\n " << p.second << endl;});
            return true;
        }
        i++;
        j = 0;
        if (status == CommandLineParserAutoMata::STATUS4) {
            status = CommandLineParserAutoMata::STATUS0;
            auto it = g_commandLineValueMap.find(command);
            if (it != g_commandLineValueMap.end()) {
                it->second.curValue = value;
                it->second.isDefault = false;
                it->second.name = command;
                command.clear();
                value.clear();
            } else {
                cout << "parser command line error, line:" << __LINE__ <<endl;
                return false;
            }
            continue;
        } else if (status == CommandLineParserAutoMata::STATUS1) {
            cout << "parser command line error, line:" << __LINE__ <<endl;
            return false;
        }
        status = CommandLineParserAutoMata::STATUS0;
        if (i > argc) {
            cout << "parser command line error, line:" << __LINE__ <<endl;
            return false;
        }
        while (argv[i][j]) {
            if (status == CommandLineParserAutoMata::STATUS0) {
                if (argv[i][j] == '=') {
                    ++j;
                    status = CommandLineParserAutoMata::STATUS1;
                } else {
                    status = CommandLineParserAutoMata::STATUS2;
                    value += argv[i][j];
                    ++j;
                }
            } else if (status == CommandLineParserAutoMata::STATUS1) {
                status = CommandLineParserAutoMata::STATUS2;
                value += argv[i][j];
                ++j;
            } else if (status == CommandLineParserAutoMata::STATUS2) {
                value += argv[i][j];
                ++j;
            }
        }
        ++i;
        j = 0;
        if (status == CommandLineParserAutoMata::STATUS2) {
            status = CommandLineParserAutoMata::STATUS0;
            auto it = g_commandLineValueMap.find(command);
            if (it != g_commandLineValueMap.end()) {
                it->second.curValue = value;
                it->second.isDefault = false;
                it->second.name = command;
                command.clear();
                value.clear();
            } else {
                cout << "parser command line error, line:" << __LINE__ <<endl;
                return false;
            }
            continue;
        }
        status = CommandLineParserAutoMata::STATUS0;
        while (argv[i][j]) {
            if (status == CommandLineParserAutoMata::STATUS0) {
                status = CommandLineParserAutoMata::STATUS1;
                value += argv[i][j];
                ++j;
            } else if (status == CommandLineParserAutoMata::STATUS1) {
                value += argv[i][j];
                ++j;
            }
        }
        auto it = g_commandLineValueMap.find(command);
        if (it != g_commandLineValueMap.end()) {
            it->second.curValue = value;
            it->second.isDefault = false;
            it->second.name = command;
            command.clear();
            value.clear();
        } else {
            cout << "parser command line error, line:" << __LINE__ <<endl;
            return false;
        }
        ++i;
        status = CommandLineParserAutoMata::STATUS0;
    }
    return true;
}

void GetAllFlags(vector<CommandLineFlagInfo> &vec)
{
    for (auto &s : g_commandLineValueMap) {
        vec.push_back(s.second);
    }
}

bool GetCommandLineFlagInfo(const string flagName, CommandLineFlagInfo &info)
{
    auto it = g_commandLineValueMap.find(flagName);
    if(it == g_commandLineValueMap.end()) {
        return false;
    }
    info = it->second;
    return true;
}

bool InitGlobalFlag(string command, string defaultValue, string description)
{
    g_commandLineValueMap[command] = CommandLineFlagInfo{"", move(defaultValue), true, command};
    g_commandLineDescriptionMap[command] = move(description);
    return true;
}
int main(int argc, char* argv[])
{
    if (!ParseCommandLineFlags(argc, argv)) {
        cout<<"parse command line error"<<endl;
        return 0;
    }
    vector<CommandLineFlagInfo> vec;
    GetAllFlags(vec);
    for (auto &v : vec) {
        cout << v.name << " : " << v.curValue << endl;
    }
    return 0;
}