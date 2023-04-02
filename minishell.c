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

//ls | sort | wc

/* OUR FUNCTION */
void initialize_command(int *fd,char**command,int p){
	int pid;
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
			getCompleteCommand(&command,p);
			execvp(command[0],command);
			perror("execvp");
			exit(-1);
		default:
			close(fd[1]);
			wait(NULL);
			close(0);
			dup(fd[0]);
			close(fd[0]);
			execvp(command[1],command);
			perror("execvp");
			exit(-1);	
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
			}
			else {
				print_command(argvv, filev, in_background);
				
				// WE START HERE
				int main_pid;
				main_pid = fork();
				if (main_pid == 0){
					if (command_counter == 1){
						getCompleteCommand(argvv,command_counter-1);
						execvp(argv_execvp[0],argv_execvp);
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
								printf("LLEGUE");
								wait(NULL);
								close(0);
								dup(fd[0]);
								close(fd[0]);
								getCompleteCommand(argvv,1);
								execvp(argv_execvp[0],argv_execvp);
								perror("execvp");
								exit(-1);	
						}
					}
				}
				else{
					wait(NULL);
				}
				
				
			}
		}
	}
	
	return 0;
}
