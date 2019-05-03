#include "carch.h"

void printUsage()
{
  printf("Usage: \n");
  printf("pack <archive> <file> ... <file> - pack files into archive \n");
  printf("list <archive> - list archive file contents and information \n");
  printf("add <archive> <file> - add file to archive \n");
  printf("strip <archive> <archived file> - remove archived file from archive \n");
  printf("help - display this help\n");
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
    const int arch_f = open(argv[2], O_WRONLY | O_EXCL | O_CREAT, S_IRUSR | S_IWUSR);

    if(arch_f == -1)
    {
      if(errno == EEXIST)
      {
        printf("Failed to create the archive: File exists.\n");
        exit(-1);
      }
    }
    struct stat arch_stat;
    off_t arch_size = 0;
    r = fstat(arch_f, &arch_stat);
    if(r < 0)
    {
      printf("Failed to stat the archive file\n");
      exit(r);
    }
    arch_size = arch_stat.st_size;
    for (int i = 3; i < argc; ++i) {
      struct stat f_stat;
      char buf[1024];
      int fd = open(argv[i], O_RDONLY);
      if(fd < -1)
      {
        printf("Failed to open file %s\n",argv[i]);
        exit(-1);
      }
      r = fstat(fd, &f_stat);
      if(r < 0)
      {
        printf("Failed to stat file %s\n",argv[i]);
        exit(r);
      }

      arch_size = arch_size + sizeof(f_stat.st_size) + (strlen(argv[i]) + 1) + f_stat.st_size;
      // r = ftruncate(arch_f, arch_size);
      r = write(arch_f, argv[i], strlen(argv[i]));
      if(r <= 0)
      {
        printf("Failed writing the file name to archive.\n");
        exit(-1);
      }
      r = write(arch_f, "\0", 1);
      if(r <= 0)
      {
        printf("Failed writing to the archive.\n");
        exit(-1);
      }
      r = write(arch_f, &f_stat.st_size ,sizeof(off_t));
      if(r <= 0)
      {
        printf("Failed writing to the archive.\n");
        exit(-1);
      }
      ssize_t numRead;
      while((numRead = read(fd, buf, 1024)) > 0)
      {
        if (write(arch_f, buf, numRead) != numRead)
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
    close(arch_f);

  } else if(strcmp(argv[1], "list") == 0)
  {
    if(argc < 3)
    {
      printUsage();
      exit(0);
    }
    const int arch_f = open(argv[2], O_RDONLY);
    if(arch_f < 0)
    {
      printf("Failed to open the archive.\n");
      exit(-1);
    }
    char buf[1024];
    struct stat arch_stat;
    r = fstat(arch_f, &arch_stat);
    FILE* arch_ff = fdopen(arch_f,"rb");
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
