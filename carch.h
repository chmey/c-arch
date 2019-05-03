#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void printUsage();
void addFileToArchive(const int arch_fd, const char* fileName);
const int openArchive(const char* fileName);

struct f_header {
  const char* fileName;
  off_t fileSize;
};
