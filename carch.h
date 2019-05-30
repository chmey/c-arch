#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

struct Header {
  char file_name[100];
  char file_size[sizeof(off_t)];
};

void printUsage();
void addFileToArchive(const int arch_fd, const char* fileName);
const int openArchive(const char* fileName);
// unpacking: memory mapped file
// try aio
// save integrity
