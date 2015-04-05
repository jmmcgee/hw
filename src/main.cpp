#include <unistd.h>
#include <stdlib.h>

#include <string>
#include <iostream>

#include "noncanmode.h"

using namespace std;

void displayPrompt();

int main(int argc, char *argv[])
{
  SetNonCanonicalMode(STDIN_FILENO, new termios);
  displayPrompt();
}

void displayPrompt()
{
  // Display prompt
  char* cpath = get_current_dir_name();
  string path(cpath != nullptr ? cpath : "");
  free(cpath);

  if(path.size() > 16) {
    size_t lastSep = path.rfind('/');
    path = "/..." + path.substr(lastSep);
  }
  write(STDOUT_FILENO, path.data(), path.size());
  write(STDOUT_FILENO, "> ", 2);
}
