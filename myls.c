#include <stdio.h>		// Header file for system call printf
#include <unistd.h>		// Header file for system call gtcwd
#include <sys/types.h>	// Header file for system calls opendir, readdir y closedir
#include <dirent.h>
#include <stdlib.h>
//#include <linux/limits.h> // Sometimes needed to be able to use PATH_MAX

int main(int argc, char *argv[]){
  char **cwd = (char**)malloc(sizeof(char*)*PATH_MAX);
  if (argc < 2)
      *cwd = getcwd(*cwd,PATH_MAX); //Not assured that the path is smaller than the path_max size
  else
      *cwd = argv[1];
  DIR * fd_open = opendir(*cwd);
  if (fd_open == NULL) return -1; //In case the given dir is not valid
  struct dirent *current;
  while ((current = readdir(fd_open)) != NULL){
    printf("%s \n", current -> d_name);
  }
  return closedir(fd_open);
}
