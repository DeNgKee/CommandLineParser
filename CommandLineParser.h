#include <map>
#include <string>
#include <vector>
#include <iostream>
struct CommandLineFlagInfo {
    std::string curValue;
    std::string defaultValue;
    bool isDefault;
    std::string name;
};

bool InitGlobalFlag(std::string, std::string, std::string);
#define DEFINE_string(command, defaultValue, description) \
    bool ret_##command = InitGlobalFlag(#command, #defaultValue, description);

enum class CommandLineParserAutoMata {
    STATUS0,
    STATUS1,
    STATUS2,
    STATUS3,
    STATUS4
};

bool CheckNumberOrAlphbet(char ch);
bool ParseCommandLineFlags(int argc, char* argv[]);
void GetAllFlags(std::vector<CommandLineFlagInfo>&);
bool GetCommandLineFlagInfo(const std::string flagName, CommandLineFlagInfo &info);