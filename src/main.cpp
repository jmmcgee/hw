#include <dirent.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <string>
#include <vector>

#include "history.hpp"
#include "noncanmode.hpp"

using namespace std;

enum class CharType { REGULAR, NEWLINE, BACKSPACE, ESCAPE, CURSOR_UP,
    CURSOR_DOWN, CURSOR_LEFT, CURSOR_RIGHT, DELETE };

void displayPrompt();
string getUserInput(History& hist);
vector<string> tokenizeInput(string input, string delims);
string extractToken(string& input, char delim);
void evaluateCommands(string input, History& hist);

int internal_cd(const char* path);
int internal_cd();
void internal_ls(const char* path);
void internal_ls();
void internal_pwd();
void internal_history(History& hist);
void internal_exit();

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
    evaluateCommands(userInput, history);
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

void evaluateCommands(string input, History& hist) {

  // Split input string into commands
  // Assume multiple commands must be piped and delimited by '|'
  vector<string> pipedCommands = tokenizeInput(input, "|");

  // Set up piped file descriptors and open files between possibly multiple
  // commands
  size_t n_commands = pipedCommands.size();
  if (!n_commands) return;

  int tail_pipe[2];
  int head_pipe[2];
  vector<int> fileFDs;

  pid_t lastPid;
  int cd_ret = -1;

  for(size_t cmd_i = 0; cmd_i < n_commands; ++cmd_i) {
    // parse command
    string cmd = pipedCommands[cmd_i];
    string inputFile = extractToken(cmd, '<');
    string outputFile = extractToken(cmd, '>');
    vector<string> args = tokenizeInput(cmd, " ");
    size_t n_args = args.size();

    if(args[0] == "cd") {
      if(n_args > 1)
        cd_ret = internal_cd(args[1].c_str());
      else
        cd_ret = internal_cd();
    }


    if(cmd_i < n_commands - 1) {
      pipe(head_pipe);
    }

    if((lastPid = fork()) == -1)
      exit(1);

    if(lastPid == 0) {
      const char* argv[n_args + 1];
      for(size_t arg_i = 0; arg_i < n_args; ++arg_i) {
        argv[arg_i] = args[arg_i].c_str();
      }
      argv[n_args] = NULL;

      const char* cmd_path = args[0].c_str();
      char* const* cmd_argv = (char* const*) argv;

      int fileFD;

      if(inputFile.size()) {
        if((fileFD = open(inputFile.c_str(), O_RDONLY)) == -1) {
          string fileMissing = "File \"" + inputFile + "\" does not exist!\n";
          write(STDOUT_FILENO, fileMissing.data(), fileMissing.size());
          exit(74);
        };
        dup2(fileFD, STDIN_FILENO);
        close(fileFD);
      }

      if(outputFile.size()) {
        if((fileFD = open(outputFile.c_str(), O_WRONLY | O_TRUNC | O_CREAT, 0664)) == -1) {
          exit(73);
        }
        dup2(fileFD, STDOUT_FILENO);
        close(fileFD);
      }

      if(cmd_i) {
        dup2(tail_pipe[0], STDIN_FILENO);
        close(tail_pipe[0]);
        close(tail_pipe[1]);
      }

      if(cmd_i < n_commands - 1) {
        close(head_pipe[0]);
        dup2(head_pipe[1], STDOUT_FILENO);
        close(head_pipe[1]);
      }

      if(args[0] == "cd") {
        exit(cd_ret);
      } else if(args[0] == "ls") {
        if(n_args > 1)
          internal_ls(args[1].c_str());
        else
          internal_ls();
      } else if(args[0] == "pwd") {
        internal_pwd();
      } else if(args[0] == "history") {
        internal_history(hist);
      } else if(args[0] == "exit") {
        internal_exit();
      }

      if (execvp(cmd_path, cmd_argv) == -1) {
        string failedExec = "Failed to execute " + args[0] + "\n";
        write(STDOUT_FILENO, failedExec.data(), failedExec.size());
        exit(127);
      }
    } else {
      if(cmd_i) {
        close(tail_pipe[0]);
        close(tail_pipe[1]);
      }

      if(cmd_i < n_commands - 1) {
        tail_pipe[0] = head_pipe[0];
        tail_pipe[1] = head_pipe[1];
      }
    }
  }

  if(n_commands > 1) {
    close(tail_pipe[0]);
    close(tail_pipe[1]);
  }

  int status = 0;
  while(waitpid(-1, &status, 0)) {
    if(WIFEXITED(status) && (WEXITSTATUS(status) == 3)) exit(0);
    if(errno == ECHILD) break;
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

int internal_cd(const char* path) {
  if(!chdir(path))
    return 0;

  write(STDERR_FILENO, "Error changing directory.\n", 26);
  return 1;
}

int internal_cd() {
  return internal_cd(getenv("HOME"));
}

void internal_ls(const char* path) {
  char filepath[1024];
  strcpy(filepath, path);
  filepath[strlen(filepath)] = '/';

  char* filename = filepath + strlen(path) + 1;

  DIR* dirp = opendir(path);

  if(!dirp) {
    write(STDERR_FILENO, "Failed to open directory \"", 26);
    write(STDERR_FILENO, path, strlen(path));
    write(STDERR_FILENO, "\"\n", 2);
    exit(0);
  }

  struct dirent* file_info;
  struct stat file_stat;
  while((file_info = readdir(dirp))) {
    strcpy(filename, file_info->d_name);
    stat(filepath, &file_stat);

    write(STDOUT_FILENO, (S_ISDIR(file_stat.st_mode) ? "d" : "-"), 1);
    write(STDOUT_FILENO, ((S_IRUSR & file_stat.st_mode) ? "r" : "-"), 1);
    write(STDOUT_FILENO, ((S_IWUSR & file_stat.st_mode) ? "w" : "-"), 1);
    write(STDOUT_FILENO, ((S_IXUSR & file_stat.st_mode) ? "x" : "-"), 1);
    write(STDOUT_FILENO, ((S_IRGRP & file_stat.st_mode) ? "r" : "-"), 1);
    write(STDOUT_FILENO, ((S_IWGRP & file_stat.st_mode) ? "w" : "-"), 1);
    write(STDOUT_FILENO, ((S_IXGRP & file_stat.st_mode) ? "x" : "-"), 1);
    write(STDOUT_FILENO, ((S_IROTH & file_stat.st_mode) ? "r" : "-"), 1);
    write(STDOUT_FILENO, ((S_IWOTH & file_stat.st_mode) ? "w" : "-"), 1);
    write(STDOUT_FILENO, ((S_IXOTH & file_stat.st_mode) ? "x" : "-"), 1);

    write(STDOUT_FILENO, " ", 1);
    write(STDOUT_FILENO, file_info->d_name, strlen(file_info->d_name));
    write(STDOUT_FILENO, "\n", 1);
  }

  exit(0);
}

void internal_ls() {
  internal_ls(".");
}

void internal_pwd() {
  char* cpath = get_current_dir_name();
  string cwd(cpath != nullptr ? cpath : "");
  free(cpath);

  write(STDOUT_FILENO, cwd.data(), cwd.size());
  write(STDOUT_FILENO, "\n", 1);

  exit(0);
}

void internal_history(History& hist) {
  hist.show();
  exit(0);
}

void internal_exit() {
  exit(3);
}
