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
//sem_t mu;
//sem_t sem_producer;
//sem_t sem_consumer;
pthread_mutex_t mutex;
pthread_cond_t full;
pthread_cond_t empty;



int init_list_clients(const char * file, char **list_client_ops) {
    char first_line[30];
    FILE * fd_open= fopen(file,"r");
    if (fd_open == NULL) exit(-1);
    fgets(first_line,30,fd_open);
    int n_command = atoi(first_line);

    if (!n_command && strncmp(first_line,"0",2)){
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

int check_argument(char * line){
        if(!atoi(line) && strncmp(line, "0", 2)) {
                perror("Number of ATMs must be a number!!\n");
                exit(-1);
            }
}


void create_account(int num_account) {
    if(account_balance[num_account - 1] != NULL) {
        perror("Account already exists!");
        exit(-1);
    }
    account_balance[num_account - 1] = malloc(sizeof(long int));
    *account_balance[num_account - 1] = 0;
}

void error_if_account_exists(int num_account) {
    if(account_balance[num_account - 1] == NULL) {
        perror("Account does not exist!");
        exit(-1);
    }
}

void deposit(int num_account, int ammount) {
    error_if_account_exists(num_account);
    *account_balance[num_account - 1] += ammount;
    global_balance += ammount;
}

void withdraw_money(int num_account, int ammount) {
    error_if_account_exists(num_account);
    *account_balance[num_account - 1] -= ammount;
    global_balance -= ammount;
}

void transfer(int num_account1, int num_account2, int ammount) {
    error_if_account_exists(num_account1);
    error_if_account_exists(num_account2);
    *account_balance[num_account1 - 1] -= ammount;
    *account_balance[num_account2 - 1] += ammount;

}

void print_account(int num_account) {
    printf("GLOBAL BALANCE: %lld\n-----------\nACCOUNT %d\nMONEY = %ld\n-----------\n", global_balance, num_account, *account_balance[num_account - 1]);
}

void teachers_print(int num_accounts,char * instruction){
    printf("%d %s BALANCE = %ld TOTAL = %lld", bank_numop, instruction, *account_balance[num_accounts - 1], global_balance);
}

void print_all_accounts(int mark1, int mark2) {
    printf(" ACCOUNT  |  MONEY\n"
           "--------------------\n");
    for(int i=1; i <= max_accounts; i++)
        if(account_balance[i - 1] != NULL) {
            if(i == mark1) printf("\033[1;31m");
            else if(i == mark2) printf("\033[1;32m");
            printf(" %-11d %-5ld$\n", i, *account_balance[i - 1]);
            if(i == mark1 || i == mark2) printf("\033[0;29m");
        }
    printf("--------------------\n"
           " TOTAL %6s%-5lld$\n", "", global_balance);
}

void do_action(char* operation) {
    char ** line = (char **)malloc(sizeof(char*)*4);

    printf("\033[1;33m%s\n\033[0;29m", operation);
    char * op = strtok(operation," ");
    int i = 0;
    while (op != NULL && i < 4){
        line[i] = op;
        op = strtok(NULL," ");
        i++;
    }

    if (strncmp(line[0], "CREATE", 7) == 0) {
        check_argument(line[1]);

        create_account(atoi(line[1]));
        print_account(atoi(line[1]));
    } else if (strncmp(line[0], "DEPOSIT", 8) == 0) {
        for (int i = 1; i < 3;i++){
            check_argument(line[i]);
           }
        deposit(atoi(line[1]), atoi(line[2]));
        print_account(atoi(line[1]));

    } else if (strncmp(line[0], "WITHDRAW", 9) == 0){
        for (int i = 1; i < 3;i++){
            check_argument(line[i]);
           }
        withdraw_money(atoi(line[1]), atoi(line[2]));
        print_account(atoi(line[1]));
    }
    else if (strncmp(line[0], "TRANSFER", 9) == 0){
        for (int i = 1; i < 4;i++){
            check_argument(line[i]);
           }
        transfer(atoi(line[1]), atoi(line[2]), atoi(line[3]));
        print_all_accounts(atoi(line[1]), atoi(line[2]));
    }
    else if (strncmp(line[0], "BALANCE", 8) == 0){
        check_argument(line[1]);
        print_account(atoi(line[1]));
    }
    else {
            printf("UNKNOWN OPERATION: %s\n", *line);
    }
    free(line);
}

void producer(queue *q) {
    while(client_numop < n_commands){
        //sem_wait(&sem_producer);
        //sem_wait(&mu);
        pthread_mutex_lock(&mutex);
        while (queue_full(q)==0){
                pthread_cond_wait(&full, &mutex);
        }
        element o;
        o.operation = list_clients_ops[client_numop++];
        queue_put(q, &o);

        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&mutex);

        print_elems(q);

        //sem_post(&mu);
        //sem_post(&sem_consumer);
    }
    pthread_exit(0);
}

void consumer(queue *q) {
     while(bank_numop < n_commands){
        //sem_wait(&sem_consumer);

        //sem_wait(&mu);


        pthread_mutex_lock(&mutex);

        while (queue_empty(q)==0){
                pthread_cond_wait(&empty, &mutex);
        }
        element o;
        o = *queue_get(q);
        do_action(o.operation);
        printf("DID OPERATION %s\n", o.operation);
        bank_numop++;
        print_elems(q);
        pthread_cond_signal(&full);
        pthread_mutex_unlock(&mutex);


        //sem_post(&mu);

        //sem_post(&sem_producer);
    }
    pthread_exit(0);
}


int main (int argc, const char * argv[] ) {
    check_arguments(argc, argv);
    int prods = atoi(argv[2]);
    int cons = atoi(argv[3]);
    //printf("prods: %d, cons: %d\n", prods, cons);
    max_accounts = atoi(argv[4]);
    int buff_size = atoi(argv[5]);

    account_balance = malloc(sizeof(long int *)*max_accounts);

    n_commands = init_list_clients(argv[1], list_clients_ops);

    //sem_init(&mu, 0, 1);
    //sem_init(&sem_producer, 0, 1);
    //sem_init(&sem_consumer, 0, 0);
    //printf("%d\n",n_commands);
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);



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

    //sem_destroy(&mu);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);

    free(list_clients_ops);
    free(account_balance);
    return 0;
}
