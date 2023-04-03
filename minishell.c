//P2-SSOO-22/23

// MSH main file
// Write your msh source code here

//#include "parser.h"
#include <stddef.h>			/* NULL */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <pthread.h>

#define MAX_COMMANDS 8

#define PREAD 0
#define PWRITE 1


// files in case of redirection
char filev[3][64];

//to store the execvp second parameter
char *argv_execvp[8];

void siginthandler(int param)
{
        printf("****  Exiting MSH **** \n");
        //signal(SIGINT, siginthandler);
        exit(0);
}


/* Timer */
pthread_t timer_thread;
unsigned long  mytime = 0;

void* timer_run ( )
{
        while (1)
        {
                usleep(1000);
                mytime++;
        }
}

/**
 * Get the command with its parameters for execvp
 * Execute this instruction before run an execvp to obtain the complete command
 * @param argvv
 * @param num_command
 * @return
 */
void getCompleteCommand(char*** argvv, int num_command) {
        //reset first
        for(int j = 0; j < 8; j++)
                argv_execvp[j] = NULL;

        int i = 0;
        for ( i = 0; argvv[num_command][i]!= NULL; i++)
                argv_execvp[i] = argvv[num_command][i];

}

void mycalc(char ***argvv) {
    write(STDERR_FILENO, "\033[1;31mmycalc reached\n\033[0;38m", strlen("\033[1;31mmycalc reached\n\033[0;38m"));
    if(!atoi(argvv[0][1]) || !atoi(argvv[0][3])
            || (strncmp(argvv[0][2], "add", 4) && strncmp(argvv[0][2], "mul", 4) && strncmp(argvv[0][2], "div", 4)))
        exit(-1);
    int result;
    if(!strncmp(argvv[0][2], "add", 4)) {
         result = atoi(argvv[0][1]) + atoi(argvv[0][3]);
         printf("%s + %s = %u; Acc: %u\n", argvv[0][1], argvv[0][3], result, 0);
         fflush(stdout);
    } else
    if(!strncmp(argvv[0][2], "mul", 4)) {
         result = atoi(argvv[0][1]) * atoi(argvv[0][3]);
         printf("%s * %s = %u\n", argvv[0][1], argvv[0][3], result);
         fflush(stdout);
    } else
    if(!strncmp(argvv[0][2], "div", 4)) {
        int remainder = atoi(argvv[0][1]) % atoi(argvv[0][3]);
         result = atoi(argvv[0][1]) / atoi(argvv[0][3]);
         printf("%s / %s = %u; Remainder: %d\n", argvv[0][1], argvv[0][3], result, remainder);
         fflush(stdout);
    }


}


/**
 * Main sheell  Loop
 */
int main(int argc, char* argv[])
{
        /**** Do not delete this code.****/
        int end = 0;
        int executed_cmd_lines = -1;
        char *cmd_line = NULL;
        char *cmd_lines[10];

        if (!isatty(STDIN_FILENO)) {
                cmd_line = (char*)malloc(100);
                while (scanf(" %[^\n]", cmd_line) != EOF){
                        if(strlen(cmd_line) <= 0) return 0;
                        cmd_lines[end] = (char*)malloc(strlen(cmd_line)+1);
                        strcpy(cmd_lines[end], cmd_line);
                        end++;
                        fflush (stdin);
                        fflush(stdout);
                }
        }

        pthread_create(&timer_thread,NULL,timer_run, NULL);

        /*********************************/

        char ***argvv = NULL;
        int num_commands;


        while (1)
        {
                int status = 0;
                int command_counter = 0;
                int in_background = 0;
                signal(SIGINT, siginthandler);

                // Prompt
                write(STDERR_FILENO, "\033[1;33mMSH>> \033[0;38m", strlen("\033[1;34mMSH>> \033[0;37m"));

                // Get command
                //********** DO NOT MODIFY THIS PART. IT DISTINGUISH BETWEEN NORMAL/CORRECTION MODE***************
                executed_cmd_lines++;
                if( end != 0 && executed_cmd_lines < end) {
                        command_counter = read_command_correction(&argvv, filev, &in_background, cmd_lines[executed_cmd_lines]);
                }
                else if( end != 0 && executed_cmd_lines == end) {
                        return 0;
                }
                else {
                        command_counter = read_command(&argvv, filev, &in_background); //NORMAL MODE
                }
                //************************************************************************************************


                /************************ STUDENTS CODE ********************************/
                if (command_counter > 0) {
                        if (command_counter > MAX_COMMANDS){
                                printf("Error: Maximum number of commands is %d \n", MAX_COMMANDS);
                        }
                        else {
                                getCompleteCommand(argvv,command_counter-1);
                                if(!strncmp(*argvv[0], "mycalc", 7) && command_counter==1){
                                        mycalc(argvv);
                                } else {
                                print_command(argvv, filev, in_background);
                                // WE START HERE
                                int main_pid;
                                int pparent[2], pchild[2];
                                main_pid = fork();
                                if (main_pid == 0){
                                    if (*filev[0] != '0'){
                                            perror("AA");
                                            close(0);
                                            open(filev[0], O_RDONLY, 0666);
                                    }
                                    if (*filev[1] != '0'){
                                            close(1);
                                            open(filev[1],O_CREAT | O_TRUNC | O_WRONLY,0666);
                                    }
                                    if (*filev[2] != '0'){
                                            close(2);
                                            open(filev[2],O_CREAT | O_TRUNC | O_WRONLY,0666);
                                    }

                                    if (command_counter == 1){
                                        execvp(argv_execvp[0],argv_execvp);
                                        exit(-1);
                                    }
                                    if (command_counter == 2){
                                            int pid;
                                            int fd[2];
                                            pipe(fd);
                                            pid = fork();
                                            switch (pid){
                                                    case -1:
                                                            perror("fork");
                                                            exit(-1);
                                                    case 0:
                                                            close(1);
                                                            dup(fd[1]);
                                                            close(fd[1]);
                                                            close(fd[0]);
                                                            getCompleteCommand(argvv,0);
                                                            execvp(argv_execvp[0],argv_execvp);
                                                            perror("execvp");
                                                            exit(-1);
                                                    default:
                                                            close(fd[1]);
                                                            close(0);
                                                            dup(fd[0]);
                                                            close(fd[0]);
                                                            getCompleteCommand(argvv,1);
                                                            waitpid(getppid(), NULL, WEXITED);
                                                            execvp(argv_execvp[0],argv_execvp);
                                                            perror("execvp");
                                                            exit(-1);
                                            }
                                    } else {
                                        for(int i=command_counter; i>0; i--){
                                            write(STDERR_FILENO, "\033[1;31mfor executed\n\033[0;38m", strlen("\033[1;31mfor executed\n\033[0;37m"));
                                            int pid;
                                            int pnum = command_counter - i;
                                            if(i != command_counter) { // 1 or all besides command counter
                                                pparent[0] = pchild[0];
                                                pparent[1] = pchild[1];
                                                close(pparent[PREAD]); // b/c we write to the parent

                                                close(1);
                                                dup(pparent[PWRITE]); //write in pipe to parent
                                                close(pparent[PWRITE]);
                                                // if not first elem we create a child
                                            }
                                            if(i != 1){
                                                // reads from child > pipe to child
                                                pipe(pchild);
                                                pid = fork();
                                            } else { // If first elem > not create child
                                                // salida de la pipe
                                                pid = getpid();
                                                write(STDERR_FILENO, "\033[1;33mgot to first elem\n\033[0;38m", strlen("\033[1;33mgot to first elem\n\033[0;37m"));
                                            }

                                            switch (pid){
                                                case -1:
                                                        perror("fork");
                                                        exit(-1);
                                                case 0: // child
                                                        break;
                                                default:
                                                    if(i != 1) {
                                                        close(pchild[PWRITE]); // b/c we read from the child
                                                        close(0);
                                                        dup(pchild[PREAD]); //write in pipe to parent
                                                        close(pchild[PREAD]);
                                                        wait(NULL);
                                                    }
                                                    getCompleteCommand(argvv,i-1);
                                                    execvp(argv_execvp[0],argv_execvp);
                                                    perror("error somewhere");
                                            }
                                        }
                                }
                                } else {
                                        if (in_background == 0){
                                                wait(NULL);
                                        }

                                }}


                        }
                }
        }

        return 0;
}
