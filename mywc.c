#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

int main(int argc, char * argv[]){
  if (argc != 2) return -1;

  int lines = 0;
  int words = 0;
  int bytes = 0;
  int fd_open = open(argv[1],O_RDONLY);

  if (fd_open < 0) return -1;

  int current;
  bool is_space = false;
  char buffer[1];

  while ((current = read(fd_open,buffer,1) > 0)){
    bytes++;
    // detects the start of a word, if it is a space, \t or \n, we don't count the word
    if (buffer[0] != ' ' && buffer[0] != '\t' && buffer[0] != '\n'){
      words++;
      // continues reading the word as long as it doesn't end
      while((current = read(fd_open,buffer,1) > 0) && (buffer[0] != ' ' && buffer[0] != '\t' && buffer[0] != '\n')) {
        bytes++;
      }
      // counts a byte for the space, \t or \n detected
      if (current > 0) bytes++;
      // passes the spaces or \t after the word
      while(buffer[0] == '\t' || buffer[0] == ' ') {
        if (current = read(fd_open, buffer,1) > 0) bytes++;
      }
    }
    // counts line
    if (buffer[0] == '\n') {
      lines++;
    }
  }
    if (current < 0){
        close(fd_open);
        return -1;
    } else {
	printf("%d %d %d %s\n", lines, words, bytes, argv[1]);
	return 0;
}
    close(fd_open);
    printf("%d %d %d %s",lines,words,bytes,argv[1]);
    return 0;
}
