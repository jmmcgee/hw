#include "command.hpp"

#include <unistd.h>

#include <map>
#include <string>
#include <vector>

using namespace std;

Command::Command(const vector<string> args, const map<int, string> fdPaths) :
  _args(args),
  _fdPaths(fdPaths)
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

int Command::execute()
{
  // fork and only run if child
  pid_t pid = fork();
  if(pid != 0)
    return pid;

  vector<const char*> cArgs;

  for(const string& arg : _args)
    cArgs.push_back(arg.data());
  cArgs.push_back(nullptr);

  // forced cast. not dealing with this shit.
  execvp(cArgs[0], (char*const*)&cArgs[1]);

  // should never happen
  return -1;
}
