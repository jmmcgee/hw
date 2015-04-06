#include "command.hpp"

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
  if(_args.size() < argNo)
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

void Command::execute()
{
}
