#include "history.hpp"

#include <unistd.h>

#include <string>

using namespace std;

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
