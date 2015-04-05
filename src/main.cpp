#include <unistd.h>
#include <stdlib.h>

#include <string>
#include <iostream>

#include "noncanmode.h"

enum class CharType { REGULAR, NEWLINE, BACKSPACE, ESCAPE, CURSOR_UP,
    CURSOR_DOWN, CURSOR_LEFT, CURSOR_RIGHT, DELETE };

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

  bool exitFlag = false;

  auto getChar = [&]()->bool {
    int ret = read(STDIN_FILENO, &c, 1);
    switch(ret) {
      case 0:
        return false;
        break;

      case 1:
        return true;
        break;

      case -1:
      default:
        exitFlag = true;
        return false;
        break;
    }
  };

  while(!exitFlag) {
    if(!getChar())
      continue;

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

        // read rest of esc sequence
        if(!getChar()) break; // read in 2nd char
        if(c != '[') break; // 2nd char must be '['
        if(!getChar()) break; // read in 3rd char

        // Parse 3rd char
        switch(c) {
          case 'A':
            c_Type = CharType::CURSOR_UP;
            break;

          case 'B':
            c_Type = CharType::CURSOR_DOWN;
            break;

          case 'C':
            c_Type = CharType::CURSOR_LEFT;
            break;

          case 'D':
            c_Type = CharType::CURSOR_RIGHT;
            break;

          case '3':
            if(!getChar()) break; // read in 4th char

            // parse 4th char (ESC [ 3)
            switch(c) {
              case '~':
                c_Type = CharType::DELETE;
                break;

              default:
                break;
            } // 4th char (ESC [ 3)

          default:
            break;
        }
        break;

      default:
        c_Type = CharType::REGULAR;
        break;
    }

    switch(c_Type) {
      case CharType::NEWLINE:
        buf[input_count] = '\0';
        return;
        break;

      case CharType::REGULAR:
        buf[input_count] = c;
        ++input_count;
        write(STDOUT_FILENO, &c, 1);
        break;

      case CharType::DELETE:
      case CharType::BACKSPACE:
        if(input_count) {
          --input_count;
          write(STDOUT_FILENO, "\b \b", 3);
        }
        break;

      case CharType::CURSOR_UP:
        break;

      case CharType::CURSOR_DOWN:
        break;

      case CharType::CURSOR_LEFT:
        break;

      case CharType::CURSOR_RIGHT:
        break;

      default:
        break;
    }
  }
}
