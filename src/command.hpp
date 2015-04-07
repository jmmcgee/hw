#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <map>
#include <string>
#include <vector>

class Command
{
  private:
    std::vector<std::string> _args;
    std::map<int, std::string> _fdPaths;

    int* inputPipeFDPair;
    int* outputPipeFDPair;
    int inputFileFD;
    int outputFileFD;

  public:
    Command(const std::vector<std::string> args = {},
        const std::map<int, std::string> = {});

    void setArg(size_t argNo, const std::string arg);
    void addArg(const std::string arg);
    void redirect(size_t fd, const std::string path);
    void setInputPipe(int* fdPair);
    void setOutputPipe(int* fdPair);
    void setInputFile(int fd);
    void setOutputFile(int fd);
    int execute();
};
#endif
