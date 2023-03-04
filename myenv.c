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
    int fd_out = open(argv[argc], O_APPEND);
    int fd_env = open("./env.txt", O_RDONLY);
    char buffer[1];
    int pass_line = 0;
    int position = 0;
    int current;
    while ((current = read(fd_env, buffer, 1)) > 0) {
      if (strncmp(buffer, "/n", 1)) {
        pass_line = 0;
        continue;
      }
      if (pass_line == 1) continue;
      if (strncmp(buffer, &argv[1][position], 1) == 1) {
        position++;
      } else {
        if (strncmp(buffer, "=", 1) == 1 && strncmp(&argv[1][position], "/0", 1) == 1) {
          write(fd_out, argv[1], 63);
          lseek(fd_env, 1, SEEK_CUR);
          while(strncmp(buffer, "/n", 1) == 0) {
            write(fd_out, buffer, 1);
            lseek(fd_env, 1, SEEK_CUR);
          }
          write(fd_out, "/n", 1);
        }
        pass_line = 1;
      }
    }
    close(fd_out);
    close(fd_env);
    return 0;
}
