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
int client_numop = 0;
int bank_numop = 0;
long long int global_balance = 0;

 /*
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
*/

void * init_list_clients(const char * file, char **list_client_ops) {
    char n_commands[3];
    FILE * fd_open= fopen(file,"r");
    if (fd_open == NULL) exit(-1);
    fgets(n_commands,3,fd_open);
    int n_command = atoi(n_commands);
    if (!n_command && strncmp(n_commands,"0",2)){
            perror("first lie is not a number!!");
            exit(-1);
    }
    else if (n_command > 200){
            perror("Number of operations greater than 200");
            exit(-1);
    }
    char ** list_clients_ops = (char **)malloc(sizeof(char*)*n_command);
    char buff[30];
    for (int i = 0;i<=n_command;i++){
            if (fgets(buff,30,fd_open) != NULL){
                list_clients_ops[i] = malloc(sizeof(char)*strlen(buff));
                strcpy(list_clients_ops[i], buff);
                list_clients_ops[i][strlen(list_clients_ops[i])-1] = '\0';
            }

    }
    for (int i = 0;i<=n_command;i++){
            printf("%s\n", list_clients_ops[i]);
    }
    fclose(fd_open);
}

void producer() {
    printf("a");
}

void consumer() {
    printf("b");
}

int main (int argc, const char * argv[] ) {
        int prods = atoi(argv[2]);
        int cons = atoi(argv[3]);
        printf("prods: %d, cons: %d\n", prods, cons);
        int max_accounts = atoi(argv[4]);
        int buff_size = atoi(argv[5]);

        int current;

        char ** list_clients_ops;
        init_list_clients(argv[1], list_clients_ops);


        pthread_t thid_atm[prods], thid_workers[cons];

        for(int i = 0;  i < prods; i++){
            pthread_create(&thid_atm[i], NULL, (void *)producer, NULL);
        }
        for(int i = 0;  i < cons; i++){
            pthread_create(&thid_workers[i], NULL, (void *)consumer, NULL);
        }

        for(int i = 0;  i < prods; i++){
            pthread_join(thid_atm[i], NULL);
        }
        for(int i = 0;  i < cons; i++){
            pthread_join(thid_workers[i], NULL);
        }

        return 0;
}
