#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
int main(int argc, char * argv[]){
    if (argc != 2)
        return -1;

    int lines;
    int words;
    int bytes;
    int fd_open = open(argv[1],O_RDONLY);
    if (fd_open < 0)
        return -1;
    int current;
    bool is_t = false;
    char buffer[1];
    while ((current = read(fd_open,buffer,1))>0){
        bytes++;
        if (buffer[0] == ' ' || buffer[0] == '\t'){
                   words++;
            if (buffer[0] == '\t')
                is_t = true;
            else
                is_t = false;
            }
        else if (buffer[0] == '\n')
            lines++;

        }
    if (current < 0){
        close(fd_open);
        return -1;
    }
    close(fd_open);
    printf("%d %d %d %s",lines,words,bytes,argv[1]);
    return 0;
}
