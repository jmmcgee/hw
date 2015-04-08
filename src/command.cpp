#include "command.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

Command::Command(const vector<string> args, const map<int, int> fdMap) :
  _args(args),
  _fdMap(fdMap),
  _fds()
{
  for(auto el : _fdMap)
    _fds.push_back(el.second);
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

void Command::redirect(size_t fdDst, size_t fdSrc)
{
  _fdMap.insert({fdDst, fdSrc});
  _fds.push_back(fdSrc);
}

int Command::execute()
{
  // fork and only run if child
  pid_t pid = fork();
  if(pid != 0)
    return pid;

  for(auto el : _fdMap)
    dup2(el.second, el.first);

  vector<const char*> cArgs;

  for(const string& arg : _args)
    cArgs.push_back(arg.data());
  cArgs.push_back(nullptr);

  execvp(cArgs[0], (char*const*)&cArgs[0]);

  // should never happen
  return -1;
}

void Command::cleanup()
{
  for(int fd : _fds)
    close(fd);
}
