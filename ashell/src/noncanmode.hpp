#ifndef NONCANMODE_HPP
#define NONCANMODE_HPP

#include <termios.h>

void ResetCanonicalMode(int fd, struct termios *savedattributes);
void SetNonCanonicalMode(int fd, struct termios *savedattributes);
int canonical_main(int argc, char *argv[]);

#endif