#include <unistd.h>
#include <stdlib.h>

#include <string>
#include <iostream>

#include "noncanmode.h"

enum class CharType {REGULAR, NEWLINE, BACKSPACE, ESCAPE};

using namespace std;

void displayPrompt();
void getUserInput(char* buf);

int main(int argc, char *argv[])
{
  char* userInput = new char[1024];

  SetNonCanonicalMode(STDIN_FILENO, new termios);
  displayPrompt();
  getUserInput(userInput);
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

void getUserInput(char* buf)
{
  char c;
  CharType c_Type;
  size_t input_count = 0;

  while(true) {
    if(read(STDIN_FILENO, &c, 1) == -1) {
      return;
    }

    switch(c) {
      case '\n':
        c_Type = CharType::NEWLINE;
        break;

      case '\b':
      case 0x7F:
        c_Type = CharType::BACKSPACE;
        break;

      case 0x1B:
        c_Type = CharType::ESCAPE;
        break;

      default:
        c_Type = CharType::REGULAR;
        break;
    }

    switch(c_Type) {
      case CharType::REGULAR:
        buf[input_count] = c;
        ++input_count;
        write(STDOUT_FILENO, &c, 1);
        break;

      case CharType::BACKSPACE:
        if(input_count) {
          --input_count;
          write(STDOUT_FILENO, "\b \b", 3);
        }
        break;

      case CharType::ESCAPE:
        // TODO: implement escape sequences like delete (escape [ 3 ~)
        break;

      default:
        buf[input_count] = '\0';
        return;
        break;
    }
  }
}
