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
    // Checking the provided function has the expected structure (null values, integers, add/mul/div, no extra parameters...)
    if(argvv[0][1]==NULL || argvv[0][3]==NULL || argvv[0][4]!=NULL || ((!atol(argvv[0][1]) && strncmp(argvv[0][1], "0", 2)) || (!atol(argvv[0][3]) && strncmp(argvv[0][3], "0", 2))
            || (strncmp(argvv[0][2], "add", 4) && strncmp(argvv[0][2], "mul", 4) && strncmp(argvv[0][2], "div", 4)))){
        fprintf(stdout,"[ERROR] The structure of the command is mycalc <operand_1> <add/mul/div> <operand_2>\n");
    } else {
        // Initialize result
        long int result;
        // In case of add option
        if(!strncmp(argvv[0][2], "add", 4)) {
             // We need to make use of env variable Acc
             long int Acc = atol(getenv("Acc"));
             result = atol(argvv[0][1]) + atol(argvv[0][3]);
             Acc += result;
             char *buf = (char *)malloc(sizeof(long int));
             // And load the new value for the env variable
             sprintf(buf, "%ld", Acc);
             setenv("Acc",buf,1);
             fprintf(stderr,"[OK] %s + %s = %ld; Acc %ld\n", argvv[0][1], argvv[0][3], result, Acc);
        } 
        // In case of mul option
        else if(!strncmp(argvv[0][2], "mul", 4)) {
             result = atol(argvv[0][1]) * atol(argvv[0][3]);
             fprintf(stderr,"[OK] %s * %s = %ld\n", argvv[0][1], argvv[0][3], result);
        } 
        // In case of div option
        else if(!strncmp(argvv[0][2], "div", 4)) {
             // If the division is by 0 we raise an error
             if(!strncmp(argvv[0][3], "0", 2)) {
                 fprintf(stdout,"[ERROR] Cannot divide by zero\n");
             } else {
                 // Remainder is needed in div
                 int remainder = atol(argvv[0][1]) % atol(argvv[0][3]);
                 result = atol(argvv[0][1]) / atol(argvv[0][3]);
                 fprintf(stderr,"[OK] %s / %s = %ld; Remainder %d\n", argvv[0][1], argvv[0][3], result, remainder);
             }
        }
    }
}

void my_time(char ***argvv){
        // checking no extra parameters are passed on the function
        if(argvv[0][1]!=NULL){
            fprintf(stderr,"[ERROR] The structure of the command is incorrect\n");
        } else {
            int hours,minutes,seconds;
            long int aux_time= mytime/1000;
            hours = aux_time / 3600;
            minutes = (aux_time % 3600) / 60;
            seconds= ((aux_time % 3600) % 60);
            fprintf(stderr,"%02d:%02d:%02d\n", hours, minutes, seconds);
        }
}


/**
 * Main sheell  Loop
 */
int main(int argc, char* argv[])
{       setenv("Acc", "0", 0);
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
                int command_counter = 0;
                int in_background = 0;
                signal(SIGINT, siginthandler);

                // Prompt
                write(STDERR_FILENO, "MSH>>", strlen("MSH>>"));

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
                        } else {
                                if(!strncmp(*argvv[0], "mycalc", 7) && command_counter==1){
                                    mycalc(argvv);
                                }
                                else if(!strncmp(*argvv[0], "mytime", 7) && command_counter==1){
                                    my_time(argvv);
                                }
                                else {
                                // WE START THE PIPES HERE
                                int main_pid; // main pid to keep the msh running
                                int status;
                                int pparent[2], pchild[2]; // pparent: pipe with parent, pchild: pipe with child
                                main_pid = fork();

                                if (main_pid == -1) {
                                    perror("Error doing fork");
                                    exit(-1);

                                } else if (main_pid != 0) { // main parent
                                    if (in_background == 0){
                                            wait(&status);
                                    } else {
                                        fprintf(stdout,"pid of child: %d\n", main_pid);
                                    }

                                } else { // child
                                    // In case of having input/output/error redirections
                                    if (*filev[0] != '0'){
                                        close(0);
                                        if(open(filev[0], O_RDONLY, 0666) < 0){
                                            perror("Error opening the input file");
                                        };
                                    }
                                    if (*filev[1] != '0'){
                                        close(1);
                                        if(open(filev[1],O_CREAT | O_TRUNC | O_WRONLY,0666) < 0){
                                            perror("Error opening the output file");
                                        }
                                    }
                                    if (*filev[2] != '0'){
                                        close(2);
                                        if(open(filev[2],O_CREAT | O_TRUNC | O_WRONLY,0666) < 0){
                                            perror("Error opening the error file");
                                        };
                                    }
                                    // Loop for executing the command or command sequence
                                    for(int i=command_counter; i > 0; i--){
                                        int pid;
                                        if(i != command_counter) { // all besides last (... | wv) write to the pipe
                                            pparent[0] = pchild[0];
                                            pparent[1] = pchild[1];
                                            close(pparent[0]);

                                            close(1); // it will write to the parent
                                            dup(pparent[1]);
                                            close(pparent[1]);
                                        }
                                        if(i != 1){ // create pipes until it gets to the first elem (wv | ...)
                                            // creates a pipe with its child
                                            pipe(pchild);
                                            pid = fork();
                                        } else { // if first elem -> default in the switch statement
                                            pid = getpid();
                                        }

                                        switch (pid){
                                            case -1:
                                                perror("Error doing fork");
                                                exit(-1);
                                            case 0: // child
                                                // go to loop
                                                break;
                                            default:
                                                // after creating child
                                                if(i != 1) { // first elem does not have a child -> reads from input file
                                                    close(pchild[1]);
                                                    close(0);
                                                    dup(pchild[0]); // it will read from the child
                                                    close(pchild[0]);
                                                    wait(&status);
                                                }
                                                getCompleteCommand(argvv,i-1);
                                                execvp(argv_execvp[0],argv_execvp);
                                                perror("Error executing command");
                                                exit(-1);
                                        }
                                   }
                                }
                            }
                        }
                }
        }

        return 0;
}
