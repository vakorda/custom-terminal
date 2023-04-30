//OS-P3 2022-2023

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <pthread.h>
#include "queue.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>



/**
 * Entry point
 * @param argc
 * @param argv
 * @return
 */
 //CONSTANTS
 char ** ATMs;
 char * workers;
 int curr_atm = 0;
 int curr_worker = 0;
 
char * parser(const char * file){
	int n = 0;
	char buffer[1];
	int fd_open= open(file,O_RDONLY);
	if (fd_open < 0)exit(-1);
	int current;
	while ((current = read(fd_open,buffer,1) > 0)){
	    while(buffer[0] != '\n') {
	    	char words[20] = "";
	    	int i = 0;
	    	while (buffer[0] != ' ' && buffer[0] != '\n'){
	    		words[i] = buffer[0];
	      		current = read(fd_open, buffer,1);
	      		i++;
	      	}
	      	printf("words: %s\n",words);
	      	if (strncmp(words, "CREATE", 7)==0){
	      		printf("LLEGUE A CREATE\n");
	      		current = read(fd_open, buffer,1);
	      		char words[5] = "";
	      		int i = 0;
		    	while (buffer[0] != ' ' && buffer[0] != '\n'){
		    		words[i] = buffer[0];
		      		current = read(fd_open, buffer,1);
		      		i++;
		      	}
		      	printf("%s\n",words);
		      	ATMs[curr_atm] = words;
		      	curr_atm += 1;
	      	}
	      	else if (strncmp(words, "DEPOSIT", 8)==0){
	      		printf("LLEGUE A DEPOSIT\n");
	      		current = read(fd_open, buffer,1);
	      		char words[5] = "";
	      		int i = 0;
		    	while (buffer[0] != '\n'){
		    		if (buffer[0] != ' '){ 
		    		words[i] = buffer[0];
		      		i++;
		      		}
		      		else{
		      		printf("%s\n",words);
		      		ATMs[curr_atm] = words;
		      		curr_atm += 1;
		      		}
		      		current = read(fd_open, buffer,1);
		      		
		      	}
		      	
	      		
	      	}
	      	else if (strncmp(words, "WITHDRAW", 9)==0){
	      		printf("LLEGUE A WITHDRAW\n");
	      	}
	      	else if (strncmp(words, "TRANSFER", 9)==0){
	      		printf("LLEGUE A TRANSFER\n");
	      	}
	      	else if (strncmp(words, "BALANCE", 8)==0){
	      		printf("LLEGUE A BALANCE\n");
	      	}
	      	else {
	      		printf("UNKNOWN OPERATION: %s\n",words);
	      	}
	      	current = read(fd_open, buffer,1);
	    }
	}
	    
}


int main (int argc, const char * argv[] ) {
	printf("%d\n",argc);
	printf("%s\n",argv[1]);
	char file[] = "example.txt";
	strcpy(file,argv[1]); 
	int prods = atoi(argv[2]);
	int cons = atoi(argv[3]);
	int max_accounts = atoi(argv[4]);	
	int buff_size = atoi(argv[5]);

	ATMs = (char **)malloc(sizeof(char*)*prods);
	workers = (char *)malloc(sizeof(char)*cons);
	char * operatins = parser((const char *)file);
	for (int i = 0;i<prods;i++){
		printf("%s\n",ATMs[i]);
	}
	return 0;
}
