#include "command.hpp"
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <map>
#include <string>
#include <vector>

using namespace std;

Command::Command(const vector<string> args, const map<int, string> fdPaths) :
  _args(args),
  _fdPaths(fdPaths),
  inputPipeFDPair(nullptr),
  outputPipeFDPair(nullptr),
  inputFileFD(0),
  outputFileFD(0)
{
}


void Command::setArg(size_t argNo, const string arg)
{
  if(_args.size() > argNo)
    _args[argNo] = arg;
}

void Command::addArg(const string arg)
{
  _args.push_back(arg);
}

void Command::redirect(size_t fd, const string path)
{
  _fdPaths.insert({fd, path});
}

void Command::setInputPipe(int* fdPair) {
  inputPipeFDPair = fdPair;
}

void Command::setOutputPipe(int* fdPair) {
  outputPipeFDPair = fdPair;
}

void Command::setInputFile(int fd) {
  inputFileFD = fd;
}

void Command::setOutputFile(int fd) {
  outputFileFD = fd;
}

int Command::execute()
{
  // fork and only run if child
  pid_t pid = fork();
  if(pid != 0) {

    return pid;
  }

  if(inputFileFD) {
    dup2(0, inputFileFD);
  } else if (inputPipeFDPair) {
    close(inputPipeFDPair[1]);
    dup2(0, inputPipeFDPair[0]);
  }

  if(outputFileFD) {
    dup2(1, outputFileFD);
  } else if (outputPipeFDPair) {
    close(inputPipeFDPair[0]);
    dup2(1, inputPipeFDPair[1]);
  }

  vector<const char*> cArgs;

  for(const string& arg : _args)
    cArgs.push_back(arg.data());
  cArgs.push_back(nullptr);

  execvp(cArgs[0], (char*const*)&cArgs[0]);

  // should never happen
  return -1;
}
