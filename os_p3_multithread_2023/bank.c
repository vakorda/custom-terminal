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
char ** list_clients_ops;
long int ** account_balance;
int n_commands;
int max_accounts;

queue *q;

pthread_mutex_t mutex;
pthread_cond_t no_full;
pthread_cond_t no_empty;

void clean_all() {
    /* Frees all global resources */
    queue_destroy(q);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&no_full);
    pthread_cond_destroy(&no_empty);


    for(int i=0; i<n_commands; i++) {
        free(list_clients_ops[i]);
    }
    free(list_clients_ops);

    for(int i=0; i<max_accounts; i++) {
        free(account_balance[i]);
    }
    free(account_balance);
}

int check_argument(char * num){
    // Check if a string can be converted into a number (for operations)
    if(!atoi(num) && strncmp(num, "0", 2)) {
            perror("Parameter is not an integer!!");
            return 0;
        }
    return 1;
}

void init_list_clients(const char * file) {
    /* Obtains operations from file and inserts the lines into list_clients_ops */
    char buff[30];
    FILE * fd_open= fopen(file,"r");
    if (fd_open == NULL) {
        perror("Cannot open input file file!");
        exit(-1);
    }
    fgets(buff,30,fd_open);
    n_commands = atoi(buff);
    buff[strlen(buff)-1] = '\0';
    if (!n_commands && strncmp(buff,"0",2)){
            perror("First line is not a number!!");
            free(account_balance);
            exit(-1);
    } else if (n_commands > 200 || n_commands < 0){
            perror("Number of operations not in range [0-200]");
            free(account_balance);
            exit(-1);
    }

    list_clients_ops = (char **)malloc(sizeof(char*)*n_commands);
    int n_lines = 0;
    // read whole file and check number of commands is correct
    while (fgets(buff,30,fd_open) != NULL){
        if (n_lines > n_commands){
                perror("MORE LINES THAN COMMANDS!!");
                free(list_clients_ops);
                free(account_balance);
                exit(-1);
        }
        list_clients_ops[n_lines] = malloc(sizeof(char)*strlen(buff));
        strcpy(list_clients_ops[n_lines], buff);
        // eliminate \n at the end of the line
        list_clients_ops[n_lines][strlen(list_clients_ops[n_lines])-1] = '\0';
        n_lines++;
        }
        if (n_lines < n_commands){
                perror("LESS LINES THAN COMMANDS!!");
                free(list_clients_ops);
                free(account_balance);
                exit(-1);
        }
    fclose(fd_open);
}

int check_arguments(int argc, const char *argv[]) {
    if(argc != 6) {
        perror("Number of arguments incorrect!!");
        exit(-1);
    }
    // check producers
    if(!atoi(argv[2]) || atoi(argv[2]) <= 0) {
        perror("Number of ATMs must be a natural number!!");
        exit(-1);
    }
    //check consumers
    if(!atoi(argv[3]) || atoi(argv[3]) <= 0) {
        perror("Number of workers must be a natural number!!");
        exit(-1);
    }
    //check max_accounts
    if(!atoi(argv[4]) || atoi(argv[4]) <= 0) {
        perror("Number of max_accounts must be a natural number!!");
        exit(-1);
    }
    //check buf_size
    if(!atoi(argv[5]) || atoi(argv[5]) <= 0) {
        perror("Size of the buffer must be a natural number!!");
        exit(-1);
    }
    return 0;
}

void create_account(int num_account) {
    if (num_account < 1 ) {
        perror("Account not in range of possible values!!");
    }
    if(num_account > max_accounts) {
        perror("Cannot create account, maximum number of accounts exceeded!!");
    }
    else if(account_balance[num_account - 1] != NULL) {
        perror("Account already exists!");
    } else {
        account_balance[num_account - 1] = malloc(sizeof(long int));
        *account_balance[num_account - 1] = 0;
    }
}

int account_exists(int num_account) {
    // checks if an account is trying to be accessed before created or not in the range allowed
    if(num_account < 1 || num_account > max_accounts) {
        perror("NUMBER OF ACCOUNTS MUST BE IN RANGE 1-MAX_ACCOUNTS");
        return 0;
    }
    else if(account_balance[num_account - 1] == NULL) {
        perror("Account does not exist!");
        return 0;
    }
    return 1;
}

void deposit(int num_account, int ammount) {
    if(account_exists(num_account)){
        *account_balance[num_account - 1] += ammount;
        global_balance += ammount;
    }
}

void withdraw_money(int num_account, int ammount) {
    if(account_exists(num_account)){
        *account_balance[num_account - 1] -= ammount;
        global_balance -= ammount;
    }
}

void transfer(int num_account1, int num_account2, int ammount) {
    if(account_exists(num_account1) && account_exists(num_account2)) {
        *account_balance[num_account1 - 1] -= ammount;
        *account_balance[num_account2 - 1] += ammount;
    }
}


void print_account(int num_account,char * instruction){
    if(account_exists(num_account)){
        printf("%d %s BALANCE = %ld TOTAL = %lld\n", bank_numop+1, instruction, *account_balance[num_account - 1], global_balance);
    }
}


void free_line(char ** line) {
    for(int i=0; i<4; i++){
        free(line[i]);
    }
    free(line);
    return;
}

void do_action(char* operation) {
    // Identifies the instruction and calls the corresponding function
    char ** line = (char **)malloc(sizeof(char*)*4);
    for(int i=0; i<4; i++){
        line[i] = malloc(sizeof(char)*30);
    }

    sscanf(operation, "%s %s %s %s", line[0], line[1], line[2], line[3]);

    if (strncmp(line[0], "CREATE", 7) == 0) {
        if (!check_argument(line[1])){
            free_line(line);
            return;
        }

        create_account(atoi(line[1]));
        print_account(atoi(line[1]),operation);
    } else if (strncmp(line[0], "DEPOSIT", 8) == 0) {
        for(int i = 1; i < 3;i++){
            if (!check_argument(line[i])){
                free_line(line);
                return;
            }
           }
        deposit(atoi(line[1]), atoi(line[2]));
        print_account(atoi(line[1]),operation);

    } else if (strncmp(line[0], "WITHDRAW", 9) == 0){
        for (int i = 1; i < 3;i++){
            if (!check_argument(line[i])){
                free_line(line);
                return;
            }
           }
        withdraw_money(atoi(line[1]), atoi(line[2]));
        print_account(atoi(line[1]),operation);
    }
    else if (strncmp(line[0], "TRANSFER", 9) == 0){
        for (int i = 1; i < 4;i++){
            if (!check_argument(line[i])){
                free_line(line);
                return;
            }
           }
        transfer(atoi(line[1]), atoi(line[2]), atoi(line[3]));
        print_account(atoi(line[2]),operation);
    }
    else if (strncmp(line[0], "BALANCE", 8) == 0){
        if (!check_argument(line[1])){
            free_line(line);
            return;
        }
        print_account(atoi(line[1]),operation);
    }
    else {
            printf("UNKNOWN OPERATION: %s\n", operation);
            free_line(line);
            return;
    }
    free_line(line);
}

void producer() {
    /* handles threads for producers */
    while(client_numop < n_commands){
        pthread_mutex_lock(&mutex);
        while (queue_full(q)==0 && client_numop < n_commands){
            pthread_cond_wait(&no_full, &mutex);
        }
        if(client_numop < n_commands){
            element o;
            o.operation = list_clients_ops[client_numop];
            queue_put(q, &o);
            client_numop++;
            pthread_cond_signal(&no_empty);
        }
        pthread_mutex_unlock(&mutex);
    }
    // tell other producers to end and exit
    pthread_cond_signal(&no_full);
    pthread_exit(0);
}

void consumer() {
    /* handles threads for consumers*/
     while(bank_numop < n_commands){
        pthread_mutex_lock(&mutex);

        while (queue_empty(q)==0 && bank_numop < n_commands){
            pthread_cond_wait(&no_empty, &mutex);
        }
        if(bank_numop < n_commands){
            element o;
            o = *queue_get(q);
            do_action(o.operation);
            bank_numop++;
            pthread_cond_signal(&no_full);
        }
        pthread_mutex_unlock(&mutex);
    }
    // tell other consumers to end and exit
    pthread_cond_signal(&no_empty);
    pthread_exit(0);
}


int main (int argc, const char * argv[] ) {
    check_arguments(argc, argv);
    int n_producers = atoi(argv[2]);
    int n_consumers = atoi(argv[3]);
    max_accounts = atoi(argv[4]);
    int buff_size = atoi(argv[5]);


    account_balance = malloc(sizeof(long int *)*max_accounts);
    init_list_clients(argv[1]);

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&no_full, NULL);
    pthread_cond_init(&no_empty, NULL);

    q = queue_init(buff_size);

    pthread_t thid_atm[n_producers], thid_workers[n_consumers];

    for(int i = 0;  i < n_producers; i++){
        pthread_create(&thid_atm[i], NULL, (void *)producer, NULL);
    }

    for(int i = 0;  i < n_consumers; i++){
        pthread_create(&thid_workers[i], NULL, (void *)consumer, NULL);
    }

    for(int i = 0;  i < n_producers; i++){
        pthread_join(thid_atm[i], NULL);
    }

    for(int i = 0;  i < n_consumers; i++){
        pthread_join(thid_workers[i], NULL);
    }

    clean_all();
    return 0;
}
