#include "carch.h"

void printUsage()
{
  printf("Usage: \n");
  printf("list <archive> - List archive file contents and information.\n");
  printf("pack <archive> <file> ... <file> - Pack files into archive. If the archive exists, files will be added.\n");
  //printf("strip <archive> <archived file> - Remove archived file from archive.\n");
  printf("help - Display this help\n");
}

const int openArchive(const char* fileName) {
  const int arch_fd = open(fileName, O_RDONLY);
  if(arch_fd < 0)
  {
    printf("Failed to open the archive.\n");
    exit(-1);
  }
  return arch_fd;
}

void addFileToArchive(const int arch_fd, const char* fileName) {
  struct stat f_stat;
  int r;
  char buf[1024];
  struct Header hdr;
  int fd = open(fileName, O_RDONLY);
  if(fd < 0)
  {
    printf("Failed to open file %s\n",fileName);
    exit(-1);
  }
  r = fstat(fd, &f_stat);
  if(r < 0)
  {
    printf("Failed to stat file %s\n", fileName);
    exit(r);
  }
  strncpy(hdr.file_name, fileName, 99);
  hdr.file_name[99] = '\0';
  memcpy(&(hdr.file_size), &(f_stat.st_size), sizeof(off_t));
  r = write(arch_fd, &hdr ,sizeof(struct Header));

  if(r <= 0)
  {
    printf("Failed writing file header to the archive.\n");
    exit(-1);
  }
  ssize_t numRead;
  while((numRead = read(fd, buf, 1024)) > 0)
  {
    if (write(arch_fd, buf, numRead) != numRead)
    {
      printf("Failed to write the entire buffer.\n");
      exit(-1);
    }
  }
  if(r < -1)
  {
    exit(-1);
  }
  close(fd);
}
int main(int argc, char const *argv[]) {
  int r;
  if(argc == 1)
  {
    printUsage();
    exit(0);
  }
  if(strcmp(argv[1], "pack") == 0)
  {
    if(argc < 4)
    {
      printUsage();
      exit(0);
    }
    const int arch_f = open(argv[2], O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);

    if(arch_f == -1)
    {
      if(errno == EEXIST)
      {
        printf("Failed to create the archive: File exists.\n");
      }
      exit(-1);
    }
    for (int i = 3; i < argc; ++i) {
      addFileToArchive(arch_f, argv[i]);
    }
    close(arch_f);

  } else if(strcmp(argv[1], "list") == 0)
  {
    if(argc < 3)
    {
      printUsage();
      exit(0);
    }
    const int arch_fd = open(argv[2], O_RDONLY, S_IRUSR | S_IWUSR);
    struct stat arch_stat;
    r = fstat(arch_fd, &arch_stat);
    printf("Archive: %s\n",argv[2]);
    printf("Size: %lu Bytes\n", arch_stat.st_size);
    printf("Files: \n");
    struct Header fhdr;
    u_int16_t i = 0;
    while((read(arch_fd, &(fhdr.file_name), sizeof(fhdr.file_name)) > 0))
    {
      ++i;
      read(arch_fd, &(fhdr.file_size), sizeof(fhdr.file_size));
      off_t f_size;
      memcpy(&f_size, &(fhdr.file_size), sizeof(off_t));
      printf("[%u]: %s - %lu Bytes \n", i ,fhdr.file_name, f_size);
      lseek(arch_fd, f_size, SEEK_CUR);
    }
  }
  return 0;
}
