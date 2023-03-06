#include <stdio.h>		// Header file for system call printf
#include <unistd.h>		// Header file for system call gtcwd
#include <sys/types.h>	// Header file for system calls opendir, readdir y closedir
#include <dirent.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#define O_RDONLY	     00
#define O_WRONLY	     01
#define O_RDWR		     02

int main(int argc, char *argv[])
{
    /* If less than two arguments (argv[0] -> program, argv[1] -> file to save environment) print an error y return -1 */
    if(argc < 3)
    {
        printf("Too few arguments\n");
        return -1;
    }
    int fd_out = open("./out.txt", O_WRONLY | O_APPEND);
    int fd_env = open("./env.txt", O_RDONLY);
    char buffer[1];
    int pass_line = 0;
    int position = 0;
    int current;
    while ((current = read(fd_env, buffer, 1)) > 0) {
      printf("%c", *buffer);
      if (*buffer == '\n') {
        printf("-----detected next line-----\n");
        pass_line = 0;
        continue;
      }
      if (pass_line == 1) continue;
      if (*buffer == argv[1][position]) {
        printf("-----detected possible-----\n");
        position++;
      } else {
        if (*buffer == '=' && argv[1][position] == '\0') {
          printf("\n\n = and \\0 obtained\n\n");
          write(fd_out, argv[1], strlen(argv[1]));
          write(fd_out, "=", 1);
          printf("current buffer: buffer1= '%c'\n", *buffer);
          lseek(fd_env, 0, SEEK_CUR);
          read(fd_env, buffer, 1);
          printf("gone to next buffer: buffer2= '%c'\n\n", *buffer);
          while(*buffer != '\n') {
            write(fd_out, buffer, 1);
            lseek(fd_env, 0, SEEK_CUR);
            read(fd_env, buffer, 1);
            printf("trying to write, current buffer= '%c'\n", *buffer);
          }
          write(fd_out, "\n", 1);
        }
        pass_line = 1;
      }
    }
    close(fd_out);
    close(fd_env);
    return 0;
}
