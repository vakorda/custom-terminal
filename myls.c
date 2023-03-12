#include <stdio.h>		// Header file for system call printf
#include <unistd.h>		// Header file for system call gtcwd
#include <sys/types.h>	// Header file for system calls opendir, readdir y closedir
#include <dirent.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
  // Allocate memory for dir
  char *dir = (char*)malloc(sizeof(char*)*PATH_MAX);
  if (argc < 2)
      dir = getcwd(dir,PATH_MAX); // Not assured that the path is smaller than the path_max size
  else
      dir = argv[1];
  DIR * fd_open = opendir(dir);
  if (fd_open == NULL) return -1; // In case the given dir is not valid
  struct dirent *current;
  // Print each file in the directory
  while ((current = readdir(fd_open)) != NULL){
    printf("%s\n", current -> d_name);
  }
  return closedir(fd_open);
}
