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
    //If less than two arguments (argv[0] -> program, argv[1] -> file to save environment) print an error and return -1
    if(argc < 3)
    {
        printf("Too few arguments\n");
        return -1;
    }
    int fd_env = open("./env.txt", O_RDONLY);
    if (fd_env < 0){
      printf("Opening environment file caused error\n");
      return -1;
    }
    int fd_out = open(argv[2], O_APPEND | O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    if (fd_out < 0 || strncmp(argv[2], "./env.txt", 10) == 0 || strncmp(argv[2], "env.txt", 10) == 0){
      printf("Opening or creating output file caused error\n");
      return -1;
    }
    char buffer[1];
    int pass_line = 0;
    int position = 0;
    int current;
    while ((current = read(fd_env, buffer, 1)) > 0) {
      printf("%c", *buffer);
      /* detects if next line has been reached */
      if (*buffer == '\n') {
        pass_line = 0;
        continue;
      }
      if (pass_line == 1) continue;
      if (*buffer == argv[1][position]) {
        position++;
      } else {
        if (*buffer == '=' && argv[1][position] == '\0') {
          write(fd_out, argv[1], strlen(argv[1]));
          write(fd_out, buffer, 1);
          read(fd_env, buffer, 1);
          while(*buffer != '\n') {
            write(fd_out, buffer, 1);
            read(fd_env, buffer, 1);
          }
          write(fd_out, "\n", 1);
        } else pass_line = 1;
        position = 0;
      }
    }
    close(fd_out);
    close(fd_env);
    return 0;
}
