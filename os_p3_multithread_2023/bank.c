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

#include <semaphore.h>



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
char ** list_clients_ops;
long int ** account_balance;
int n_commands;
int max_accounts;
sem_t mu;
sem_t sem_producer;
sem_t sem_consumer;
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

int init_list_clients(const char *file, char **list_client_ops);
int check_arguments(int argc, const char *argv[]);
void producer(queue *q);
void consumer(queue *q);

int init_list_clients(const char * file, char **list_client_ops) {
    char n_commands[3];
    FILE * fd_open= fopen(file,"r");
    if (fd_open == NULL) exit(-1);
    fgets(n_commands,30,fd_open);
    int n_command = atoi(n_commands);

    if (!n_command && strncmp(n_commands,"0",2)){
            perror("first lie is not a number!!");
            exit(-1);
    } else if (n_command > 200){
            perror("Number of operations greater than 200");
            exit(-1);
    }
    list_clients_ops = (char **)malloc(sizeof(char*)*n_command);
    char buff[30];
    for (int i = 0;i < n_command;i++){
            if (fgets(buff,30,fd_open) != NULL){
                list_clients_ops[i] = malloc(sizeof(char)*strlen(buff));
                strcpy(list_clients_ops[i], buff);
                list_clients_ops[i][strlen(list_clients_ops[i])-1] = '\0';
            }

    }
    /*for (int i = 0;i < n_command;i++){
            printf("%s\n", list_clients_ops[i]);
    }*/
    fclose(fd_open);
    return n_command;
}

int check_arguments(int argc, const char *argv[]) {
    if(argc < 6) {
        perror("Number of arguments incorrect!!\n");
        exit(-1);
    }
    // check producers
    if(!atoi(argv[2]) && strncmp(argv[2], "0", 2)) {
        perror("Number of ATMs must be a number!!\n");
        exit(-1);
    }
    //check consumers
    if(!atoi(argv[3]) && strncmp(argv[3], "0", 2)) {
        perror("Number of workers must be a number!!\n");
        exit(-1);
    }
    //check max_accounts
    if(!atoi(argv[4]) && strncmp(argv[4], "0", 2)) {
        perror("Number of max_accounts must be a number!!\n");
        exit(-1);
    }
    //check buf_size
    if(!atoi(argv[5]) && strncmp(argv[5], "0", 2)) {
        perror("Size of buffer must be a number!!\n");
        exit(-1);
    }
    return 0;
}

void producer(queue *q) {

   while(client_numop < n_commands){
        sem_wait(&sem_producer);
        sem_wait(&mu);
        element o;
        o.operation = list_clients_ops[client_numop++];
        queue_put(q, &o);
        // print_elems(q);

        sem_post(&mu);
        sem_post(&sem_consumer);
    }
    pthread_exit(NULL);
}

void create_account(int num_account) {
    if(account_balance[num_account] != NULL) {
        perror("Account already exists!");
        exit(-1);
    }
    account_balance[num_account] = malloc(sizeof(long int));
    *account_balance[num_account] = 0;
}

void error_if_account_exists(int num_account) {
    if(account_balance[num_account] == NULL) {
        perror("Account does not exist!");
        exit(-1);
    }
}

void deposit(int num_account, int ammount) {
    error_if_account_exists(num_account);
    *account_balance[num_account] += ammount;
    global_balance += ammount;
}

void withdraw_money(int num_account, int ammount) {
    error_if_account_exists(num_account);
    *account_balance[num_account] -= ammount;
    global_balance -= ammount;
}

void transfer(int num_account1, int num_account2, int ammount) {
    error_if_account_exists(num_account1);
    error_if_account_exists(num_account2);
    *account_balance[num_account1] -= ammount;
    *account_balance[num_account2] += ammount;

}

void print_account(int num_account) {
    printf("GLOBAL BALANCE: %lld\n-----------\nACCOUNT %d\nMONEY = %ld\n-----------\n", global_balance, num_account, *account_balance[num_account]);
}

void print_all_accounts(int mark1, int mark2) {
    printf(" ACCOUNT  |  MONEY\n"
           "--------------------\n");
    for(int i=1; i <= max_accounts; i++)
        if(account_balance[i] != NULL) {
            if(i == mark1) printf("\033[1;31m");
            else if(i == mark2) printf("\033[1;32m");
            printf(" %-11d %-5ld$\n", i, *account_balance[i]);
            if(i == mark1 || i == mark2) printf("\033[0;29m");
        }
    printf("--------------------\n"
           " TOTAL %6s%-5lld$\n", "", global_balance);
}

void do_action(char* operation) {
    char *line;
    int param1;
    int param2;
    int param3;
    printf("\033[1;33m%s\n\033[0;29m", operation);
    sscanf(operation, "%s %d %d %d", line, &param1, &param2, &param3);
    if (strncmp(line, "CREATE", 7) == 0) {
        create_account(param1);
        print_all_accounts(param1, -1);
    } else if (strncmp(line, "DEPOSIT", 8) == 0) {
        deposit(param1, param2);
        print_account(param1);

    } else if (strncmp(line, "WITHDRAW", 9) == 0){
        withdraw_money(param1, param2);
        print_account(param1);
    }
    else if (strncmp(line, "TRANSFER", 9) == 0){
        transfer(param1, param2, param3);
        print_all_accounts(param1, param2);
    }
    else if (strncmp(line, "BALANCE", 8) == 0){
        print_account(param1);
    }
    else {
            printf("UNKNOWN OPERATION: %s\n", line);
    }
}

void consumer(queue *q) {
    printf("b");
    while(bank_numop < n_commands){
        sem_wait(&sem_consumer);

        sem_wait(&mu);

        element o;
        o = *queue_get(q);
        bank_numop++;
        // printf("got element: %s\n", o.operation);
        do_action(o.operation);
        sem_post(&mu);

        sem_post(&sem_producer);
    }
    pthread_exit(NULL);
}

pthread_mutex_t mutex;
pthread_cond_t non_full;
pthread_cond_t non_empty;

int main (int argc, const char * argv[] ) {
    check_arguments(argc, argv);
    int prods = atoi(argv[2]);
    int cons = atoi(argv[3]);
    printf("prods: %d, cons: %d\n", prods, cons);
    max_accounts = atoi(argv[4]);
    int buff_size = atoi(argv[5]);

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&non_full, NULL);
    pthread_cond_init(&non_empty, NULL);


    account_balance = malloc(sizeof(long int *)*max_accounts);

    n_commands = init_list_clients(argv[1], list_clients_ops);

    sem_init(&mu, 0, 1);
    sem_init(&sem_producer, 0, 1);
    sem_init(&sem_consumer, 0, 0);

    queue q;
    q = *queue_init(buff_size);

    pthread_t thid_atm[prods], thid_workers[cons];

    for(int i = 0;  i < prods; i++){
        pthread_create(&thid_atm[i], NULL, (void *)producer, &q);
    }
    for(int i = 0;  i < cons; i++){
        pthread_create(&thid_workers[i], NULL, (void *)consumer, &q);
    }

    for(int i = 0;  i < prods; i++){
        pthread_join(thid_atm[i], NULL);
    }
    for(int i = 0;  i < cons; i++){
        pthread_join(thid_workers[i], NULL);
    }

    queue_destroy(&q);
    sem_destroy(&mu);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&non_full);
    pthread_cond_destroy(&non_empty);

    return 0;
}
