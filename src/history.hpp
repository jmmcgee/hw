#ifndef HISTORY_HPP
#define HISTORY_HPP

#include <string>

class History {
  private:
    std::string* history;
    size_t size;
    size_t entries;
    size_t head;
    size_t tail;
    size_t cursor;

  public:
    History(unsigned int m_size);
    ~History();

    void addEntry(std::string buf);

    void show();

    void resetCursor();
    std::string getPrevHistory();
    std::string getNextHistory();
};

#endif
