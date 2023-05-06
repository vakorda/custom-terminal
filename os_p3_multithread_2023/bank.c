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

pthread_mutex_t mutex;
pthread_cond_t no_full;
pthread_cond_t no_empty;



int init_list_clients(const char * file, char **list_client_ops) {
    char buff[30];
    FILE * fd_open= fopen(file,"r");
    if (fd_open == NULL) exit(-1);
    fgets(buff,30,fd_open);
    int n_command = atoi(buff);

    if (!n_command && strncmp(buff,"0",2)){
            perror("first lie is not a number!!");
            exit(-1);
    } else if (n_command > 200){
            perror("Number of operations greater than 200");
            exit(-1);
    }
    list_clients_ops = (char **)malloc(sizeof(char*)*n_command);
    int n_lines = 0;
    while (fgets(buff,30,fd_open) != NULL){
    	if (n_lines > n_command){
    		perror("MORE LINES THAN COMMANDS");
    		free(list_clients_ops);
    		exit(-1);
    	}
        list_clients_ops[n_lines] = malloc(sizeof(char)*strlen(buff));
        strcpy(list_clients_ops[n_lines], buff);
        list_clients_ops[n_lines][strlen(list_clients_ops[n_lines])-1] = '\0';
        n_lines++;
        }
	if (n_lines < n_command){
		perror("LESS LINES THAN COMMANDS");
		free(list_clients_ops);
		exit(-1);
	}
    fclose(fd_open);
    return n_command;
}

int check_arguments(int argc, const char *argv[]) {
    if(argc != 6) {
        perror("Number of arguments incorrect!!\n");
        exit(-1);
    }
    // check producers
    if(!atoi(argv[2]) && strncmp(argv[2], "0", 2) || atoi(argv[2]) <= 0) {
        perror("Number of ATMs must be a natural number!!");
        exit(-1);
    }
    //check consumers
    if(!atoi(argv[3]) && strncmp(argv[3], "0", 2)|| atoi(argv[3]) <= 0) {
        perror("Number of workers must be a natural number!!");
        exit(-1);
    }
    //check max_accounts
    if(!atoi(argv[4]) && strncmp(argv[4], "0", 2)|| atoi(argv[4]) <= 0) {
        perror("Number of max_accounts must be a natural number!!");
        exit(-1);
    }
    //check buf_size
    if(!atoi(argv[5]) && strncmp(argv[5], "0", 2)|| atoi(argv[5]) <= 0) {
        perror("Size of buffer must be a natual number!!");
        exit(-1);
    }
    return 0;
}

void check_argument(char * num){
	// Check if a string can be converted into a number (for parser's parameters)
        if(!atoi(num) && strncmp(num, "0", 2)) {
                perror("Parameter is not an integer\n");
                exit(-1);
            }
}

void create_account(int num_account) {
    if(num_account < 1 || num_account > max_accounts) {
        perror("Cannot create account, maximum number of accounts exceeded!!");
        exit(-1);
    }
    else if(account_balance[num_account - 1] != NULL) {
        perror("Account already exists!");
        exit(-1);
    }
    account_balance[num_account - 1] = malloc(sizeof(long int));
    *account_balance[num_account - 1] = 0;
}

void error_if_account_exists(int num_account) {
    // Funcion for checking if an account is trying to be accessed before created
    if(num_account < 1 || num_account > max_accounts) {
        perror("NUMBER OF ACCOUNTS MUST BE IN RANGE 1-MAX_ACCOUNTS");
        exit(-1);
    }
    else if(account_balance[num_account - 1] == NULL) {
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


void teachers_print(int num_accounts,char * instruction){
    // Function for printing
    printf("%d %s BALANCE = %ld TOTAL = %lld\n", bank_numop+1, instruction, *account_balance[num_accounts - 1], global_balance);
}


void do_action(char* operation) {
    // Function in charge of identifying the instruction and calling the corresponding function
    char aux[30];
    strcpy(aux,operation);
    char ** line = (char **)malloc(sizeof(char*)*4);
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
        teachers_print(atoi(line[1]),aux);
    } else if (strncmp(line[0], "DEPOSIT", 8) == 0) {
        for (int i = 1; i < 3;i++){
            check_argument(line[i]);
           }
        deposit(atoi(line[1]), atoi(line[2]));
        teachers_print(atoi(line[1]),aux);

    } else if (strncmp(line[0], "WITHDRAW", 9) == 0){
        for (int i = 1; i < 3;i++){
            check_argument(line[i]);
           }
        withdraw_money(atoi(line[1]), atoi(line[2]));
        teachers_print(atoi(line[1]),aux);
    }
    else if (strncmp(line[0], "TRANSFER", 9) == 0){
        for (int i = 1; i < 4;i++){
            check_argument(line[i]);
           }
        transfer(atoi(line[1]), atoi(line[2]), atoi(line[3]));
        teachers_print(atoi(line[2]),aux);
    }
    else if (strncmp(line[0], "BALANCE", 8) == 0){
        check_argument(line[1]);
        teachers_print(atoi(line[1]),aux);
    }
    else {
            printf("UNKNOWN OPERATION: %s\n", aux);
    }
    free(line);
}

void producer(queue *q) {
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
    pthread_cond_signal(&no_full);
    pthread_exit(0);
}

void consumer(queue *q) {
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
    pthread_cond_signal(&no_empty);
    pthread_exit(0);
}


int main (int argc, const char * argv[] ) {
    check_arguments(argc, argv);
    int prods = atoi(argv[2]);
    int cons = atoi(argv[3]);
    max_accounts = atoi(argv[4]);
    int buff_size = atoi(argv[5]);

    account_balance = malloc(sizeof(long int *)*max_accounts);

    n_commands = init_list_clients(argv[1], list_clients_ops);

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&no_full, NULL);
    pthread_cond_init(&no_empty, NULL);

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

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&no_full);
    pthread_cond_destroy(&no_empty);

    free(list_clients_ops);
    free(account_balance);
    return 0;
}
