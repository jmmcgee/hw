#include <unistd.h>
#include <stdlib.h>

#include <string.h>
#include <string>
#include <iostream>

#include "noncanmode.h"

enum class CharType { REGULAR, NEWLINE, BACKSPACE, ESCAPE, CURSOR_UP,
    CURSOR_DOWN, CURSOR_LEFT, CURSOR_RIGHT, DELETE };

using namespace std;

class History {
  private:
    string* history;
    size_t size;
    size_t entries;
    size_t head;
    size_t tail;
    size_t cursor;

  public:
    History(unsigned int m_size);
    ~History();

    void addEntry(char* buf);

    void show();

    void resetCursor();
    string getPrevHistory();
    string getNextHistory();
};

History::History(unsigned int m_size):
  history(new string[m_size]),
  size(m_size),
  entries(0),
  head(0),
  tail(0),
  cursor(0)
{
}

History::~History()
{
  delete[] history;
}

void History::addEntry(char* buf)
{
  if (entries < size) {
    history[tail] = string(buf);
    ++tail;
    ++entries;
  } else {
    head = (head + 1) % size;
    tail = (size + head - 1) % size;
    history[tail] = string(buf);
  }
}

void History::show() {
  string entry;
  string numbering;

  for (size_t i = 0; i < entries; ++i) {
    entry = history[(head + i) % size];
    numbering = to_string(i) + " ";
    write(STDOUT_FILENO, numbering.data(), numbering.size());
    write(STDOUT_FILENO, entry.data(), entry.size());
    write(STDOUT_FILENO, "\n", 1);
  }
}

void History::resetCursor() {
  cursor = entries;
}

string History::getPrevHistory() {
  if(cursor)
    return history[(head + --cursor) % size];

  return "";
}

string History::getNextHistory() {
  if(cursor < entries)
    if(++cursor < entries)
      return history[(head + cursor) % size];

  return "";

}

void inputLoop();
void displayPrompt();
void getUserInput(char* buf, History* hist);
void processUserInput(char* buf);

int main(int argc, char *argv[])
{
  struct termios SavedTermAttributes;

  SetNonCanonicalMode(STDIN_FILENO, &SavedTermAttributes);

  inputLoop();
}

void inputLoop()
{
  char userInput[1024];
  History history = History(10);

  while(true) {
    displayPrompt();
    getUserInput(userInput, &history);

    if(*userInput) history.addEntry(userInput);

    processUserInput(userInput);
  }
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

void getUserInput(char* buf, History* hist)
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

  string hist_input;

  hist->resetCursor();

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
        write(STDOUT_FILENO, "\n", 1);
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
        } else {
          write(STDOUT_FILENO, "\a", 1);
        }
        break;

      case CharType::CURSOR_UP:
        if((hist_input = hist->getPrevHistory()).size()) {
          for (size_t i = input_count; i; --i) {
            write(STDOUT_FILENO, "\b \b", 3);
          }
          write(STDOUT_FILENO, hist_input.data(), hist_input.size());

          strcpy(buf, hist_input.data());
          input_count = hist_input.size();
        } else {
          write(STDOUT_FILENO, "\a", 1);
        }
        break;

      case CharType::CURSOR_DOWN:
        if((hist_input = hist->getNextHistory()).size()) {
          for (size_t i = input_count; i; --i) {
            write(STDOUT_FILENO, "\b \b", 3);
          }
          write(STDOUT_FILENO, hist_input.data(), hist_input.size());

          strcpy(buf, hist_input.data());
          input_count = hist_input.size();
        } else {
          for (size_t i = input_count; i; --i) {
            write(STDOUT_FILENO, "\b \b", 3);
          }

          buf[0] = '\0';
          input_count = 0;
        }
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

void processUserInput(char* buf) {
  cout << "===== You entered this following: =====" << endl;
  cout << buf << endl;
}
