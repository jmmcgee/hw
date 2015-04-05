#include <unistd.h>
#include <stdlib.h>

#include <string>
#include <iostream>

#include "noncanmode.h"

using namespace std;

void displayPrompt();
void getUserInput();

int main(int argc, char *argv[])
{
  SetNonCanonicalMode(STDIN_FILENO, new termios);
  displayPrompt();
  getUserInput();
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

void getUserInput()
{
  char c;
  size_t input_count = 0;

  // TODO: implement delete (escape-[-3-~)

  while((read(STDIN_FILENO, &c, 1) != -1) && (c != '\n')) {
    switch(c) {
      case '\b':
        if(input_count) {
          --input_count;
          write(STDOUT_FILENO, "\b \b", 3);
        }
        break;
      default:
        ++input_count;
        write(STDOUT_FILENO, &c, 1);
    }
  }
}
