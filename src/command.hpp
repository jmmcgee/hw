#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <map>
#include <string>
#include <vector>

class Command
{
  private:
    std::vector<std::string> args;
    std::map<int, std::string> fdPaths;

  public:
    Command();
    Command(std::string args);

    void setArg(size_t argNo, std::string arg);
    void addArg(std::string arg);
    void redirect(size_t fd, std::string path);
    void execute();
};
#endif
