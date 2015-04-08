#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <map>
#include <string>
#include <vector>

class Command
{
  private:
    std::vector<std::string> _args;
    std::map<int, int> _fdMap;
    std::vector<int> _fds;

  public:
    Command(const std::vector<std::string> args = {},
        const std::map<int, int> = {});

    void setArg(size_t argNo, const std::string arg);
    void addArg(const std::string arg);
    void redirect(size_t fdDst, size_t fdSrc);
    int execute();
    void cleanup();
};
#endif
