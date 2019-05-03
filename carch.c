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
  r = write(arch_fd, fileName, strlen(fileName));
  if(r <= 0)
  {
    printf("Failed writing the file name to archive.\n");
    exit(-1);
  }
  r = write(arch_fd, "\0", 1);
  if(r <= 0)
  {
    printf("Failed writing to the archive.\n");
    exit(-1);
  }
  r = write(arch_fd, &f_stat.st_size ,sizeof(off_t));
  if(r <= 0)
  {
    printf("Failed writing to the archive.\n");
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
        exit(-1);
      }
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
    const int arch_fd = openArchive(argv[2]);
    char buf[1024];
    struct stat arch_stat;
    r = fstat(arch_fd, &arch_stat);
    FILE* arch_ff = fdopen(arch_fd,"rb");
    printf("Archive: %s\n",argv[2]);
    printf("Size: %lu Bytes\n", arch_stat.st_size);
    printf("Files: \n");
    int ch, i;
    off_t f_size;
    while(1)
    {
      ch = fgetc(arch_ff);
      if(feof(arch_ff)) break;
      ungetc(ch, arch_ff);
      i = 0;
      while ((ch = fgetc(arch_ff)) != '\0' && ch != EOF) {
        buf[i++] = ch;
      }
      buf[i] = '\0';
      r = fread(&f_size, sizeof(off_t), 1, arch_ff);
      printf("%s - %lu Bytes \n",buf, f_size);
      fseek(arch_ff, f_size, SEEK_CUR);
    }
  }
  return 0;
}
