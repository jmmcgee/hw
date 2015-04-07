#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include <iostream>
#include <string>
#include <vector>

#include "command.hpp"
#include "history.hpp"
#include "noncanmode.hpp"

using namespace std;

enum class CharType { REGULAR, NEWLINE, BACKSPACE, ESCAPE, CURSOR_UP,
    CURSOR_DOWN, CURSOR_LEFT, CURSOR_RIGHT, DELETE };

void displayPrompt();
string getUserInput(History& hist);
vector<string> tokenizeInput(string input, string delims);
string extractToken(string& input, char delim);
void evaluateCommands(string input);

int main(int argc, char *argv[])
{
  struct termios SavedTermAttributes;
  string userInput;
  History history = History(10);



  SetNonCanonicalMode(STDIN_FILENO, &SavedTermAttributes);
  while(true) {
    displayPrompt();
    userInput = getUserInput(history);

    if(userInput.size() == 0)
      continue;

    history.addEntry(userInput);
    evaluateCommands(userInput);
  }
  ResetCanonicalMode(STDIN_FILENO, &SavedTermAttributes);

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

string getUserInput(History& hist)
{
  string buf;
  char c;

  CharType c_Type;
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

  hist.resetCursor();

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
        write(STDOUT_FILENO, "\n", 1);
        return buf;
        break;

      case CharType::REGULAR:
        buf.push_back(c);
        write(STDOUT_FILENO, &c, 1);
        break;

      case CharType::DELETE:
      case CharType::BACKSPACE:
        if(buf.size() > 0) {
          buf.resize(buf.size()-1);
          write(STDOUT_FILENO, "\b \b", 3);
        } else {
          write(STDOUT_FILENO, "\a", 1);
        }
        break;

      case CharType::CURSOR_UP:
        if((hist_input = hist.getPrevHistory()).size()) {
          for (size_t i = buf.size(); i; --i) {
            write(STDOUT_FILENO, "\b \b", 3);
          }
          buf = hist_input;
          write(STDOUT_FILENO, buf.data(), buf.size());
        } else {
          write(STDOUT_FILENO, "\a", 1);
        }
        break;

      case CharType::CURSOR_DOWN:
        if((hist_input = hist.getNextHistory()).size()) {
          for (size_t i = buf.size(); i; --i) {
            write(STDOUT_FILENO, "\b \b", 3);
          }
          buf = hist_input;
          write(STDOUT_FILENO, buf.data(), buf.size());
        } else {
          for (size_t i = buf.size(); i; --i) {
            write(STDOUT_FILENO, "\b \b", 3);
          }

          buf[0] = '\0';
          buf.clear();
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
  return "";
}

void evaluateCommands(string input) {
  vector<Command> commands;

  vector<int*> pipes;
  vector<int> fileFDs;

  // Split input string into commands
  // Assume multiple commands must be piped and delimited by '|'
  vector<string> pipedCommands = tokenizeInput(input, "|");

  // Set up piped file descriptors and open files between possibly multiple
  // commands
  size_t n_commands = pipedCommands.size();
  if (!n_commands) return;

  string commandStr = pipedCommands[0];
  string inputFile = extractToken(commandStr, '<');
  string outputFile = extractToken(commandStr, '>');

  Command cmd = Command(tokenizeInput(commandStr, " "));

  int fd;
  if(inputFile.size()) {
    fd = open(inputFile.c_str(), O_RDONLY);
    cmd.setInputFile(fd);
    fileFDs.push_back(fd);
  }
  if(outputFile.size()) {
    fd = open(outputFile.c_str(), O_WRONLY|O_CREAT, 0664);
    cmd.setOutputFile(fd);
    fileFDs.push_back(fd);
  }

  commands.push_back(cmd);

  for(size_t i = 1; i < n_commands; ++i) {
    commandStr = pipedCommands[i];
    inputFile = extractToken(commandStr, '<');
    outputFile = extractToken(commandStr, '>');

    cmd = Command(tokenizeInput(commandStr, " "));

    if(inputFile.size()) {
      fd = open(inputFile.c_str(), O_RDONLY);
      cmd.setInputFile(fd);
      fileFDs.push_back(fd);
    }
    if(outputFile.size()) {
      fd = open(outputFile.c_str(), O_WRONLY|O_CREAT, 0664);
      cmd.setOutputFile(fd);
      fileFDs.push_back(fd);
    }

    commands.push_back(cmd);

    int* fdPair = new int[2];
    pipe(fdPair);
    pipes.push_back(fdPair);

    commands[i].setInputPipe(fdPair);
    commands[i - 1].setOutputPipe(fdPair);
  }

  int lastPid;
  int status;

  for(Command cmd : commands) {
    lastPid = cmd.execute();
    while(wait(&status) != lastPid);
  }

  for(int* fdPair : pipes) {
    close(fdPair[0]);
    close(fdPair[1]);
    delete[] fdPair;
  }

  for(int fd : fileFDs) {
    close(fd);
  }
}

string extractToken(string& input, char delim) {
  size_t delimStart = input.find_first_of(delim);
  if(delimStart == string::npos) return "";

  size_t tokenStart = input.find_first_not_of(" ", delimStart + 1);
  size_t tokenEnd = input.find_first_of(" ", tokenStart);
  if(tokenEnd == string::npos) tokenEnd = input.size();

  string token = input.substr(tokenStart, tokenEnd - tokenStart);

  input.replace(delimStart, tokenEnd - delimStart, "");

  return token;
}

vector<string> tokenizeInput(string input, string delims) {
  // First tokenize input
  vector<string> tokens;

  size_t pos = 0;
  while(pos < input.size()) {
    size_t tokenStart = input.find_first_not_of(delims, pos);
    size_t tokenEnd = input.find_first_of(delims, tokenStart);

    // ensure in bounds
    if(tokenStart >= input.size())
      break;

    string token = input.substr(tokenStart, tokenEnd - tokenStart);
    tokens.push_back(token);
    pos = tokenEnd;
  }

  return tokens;
}
