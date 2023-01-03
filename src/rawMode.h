#pragma once

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

void die(const char* s);
void enableRawMode();
void disableRawMode();
