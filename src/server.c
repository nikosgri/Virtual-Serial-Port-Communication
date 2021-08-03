#include <errno.h>
#include <fcntl.h> /*file control*/
#include <stdio.h> /*standard input output*/
#include <stdlib.h>
#include <string.h> /* string functions*/
#include <sys/types.h>
#include <termios.h> /* posix terminal*/
#include <unistd.h>
#include "toptions/term_options.h" /* Header file full location*/

/* Special Defines */
#define SIZE 1000
#define REGS_NUM 100
#define RESPONSE_SIZE 12

/*data structures*/
typedef enum operation {
    INSERT, /*0*/
    READ, /*1*/
    INFO, /*2*/
    ERROR /*3*/
} operation_t;
typedef struct request {
    operation_t operation; /* Operation Type.*/
    int ID; /* Registers ID.*/
    int new_item[REGS_NUM]; /* This array  handles counting every inserting item. This will help not to check every time which ID is mentioned.In first place is reg's 1 counter and in second reg's 2.*/
} request_t;

int** regs; /* 2D array where handles the register values.*/
char* informations[REGS_NUM]; /*Informations about registers*/
int count_registers = 0;
char* receive_message;

/**
*@name  Initialize       : If client imports a new register then it handles to initialize proper parameters
*@param receive_message  : A pointer to receiving message 
*@retval                 : Nothing 
*/
void initialize(char* receive_message)
{

    /*Message comes with the key-word import.<string>*/

    char *token = NULL, *useful_info = NULL, copy_message[10];
    char* store_tokens[5];
    int stable = 0;

    useful_info = (char*)malloc(100 * sizeof(char));
    token = strtok(receive_message, "."); /* split message */
    int i = 0;
    while (token != NULL) {
        i++;
        store_tokens[i] = token;
        token = strtok(NULL, ".");
    }
    useful_info = store_tokens[i]; /* extract <string> ,where string is the bounds of REGISTER */

    count_registers++; /*Inform  the proper param ,which  handles informations about registers that new message received*/

    informations[count_registers] = useful_info; /* inser data into it */
}
/**
*@name  StartUp : Handles to set up  parameters and initialize first values 
*@param request : Information about registers
*@retval        : Nothing 
*/
void start_up(request_t* request)
{

    regs = (int**)malloc(REGS_NUM * sizeof(int*)); /*Initializing registers.*/
    if (regs == NULL) {
        printf("Cant create data base for registers\n");
        exit(1);
    }

    for (int i = 1; i <= REGS_NUM; i++) {
        regs[i] = (int*)malloc(SIZE * sizeof(int));
        if (regs[i] == NULL) {
            exit(1);
        }
    }
    memset(regs, 0, sizeof(regs));
    for (int i = 0; i < 100; i++) {
        request->new_item[i] = 0;
    }
    /*Setting first values of the registers.*/
    regs[1][request->new_item[1]] = 0;
    regs[2][request->new_item[2]] = 1;

    informations[1] = "0-16535";
    informations[2] = "1|2|3";
    count_registers = 2;
}
/**
*@name isValid  : Parses the value where client wants to insert at the specific register and determines if it is valid or not according to specific orders.
*@param value   : The specific value where client wants to insert
*@param request : Information about registers
*@retval 	    : 1 or 0.
*/
int is_valid(char* value, request_t* request)
{
    /* According to ID , reads specific orders about registers information , split the bounds of REGS-num and stores first and last value , in order
    to check when the value that client wants to be inserted is valid or not. */

    char* first_num = NULL,* last_num = NULL,* token=NULL;
    char  useful_info[10];
    char* search;
    int num;

    sprintf(useful_info, "%s", informations[request->ID]);

    search = strchr(useful_info,'|'); /* Scan message in order to find distinct number symbol. */
    num = atoi(value);
    if(search){ /* Distinct Numbers */
        token = strtok(useful_info,"|");
        while(token!=NULL){
            if(num == atoi(token)){
                return 1;
            }
            token = strtok(NULL,"|");
        }
        
        return 0;
    }else{ /* Bounds */
        first_num = strtok(useful_info,"-");
        last_num = strtok(NULL,"-");
        if(num < atoi(first_num) || num > atoi(last_num) ){
            return 0;
        }else{
            return 1;
        }
    }
}
/**
*@name parse_request : Parses a received message extract informations from it. Specifically , split receiving message and determines the ID and operation of it.
*@param buffer       : A pointer to the received message
*@retval             : Initializes request on Success. NULL on Error.
*/
request_t* parse_request(char* buffer)
{

    char* token = NULL,* value = NULL;
    int item_counter;
    request_t* request = NULL;

    if (!buffer)
        return NULL;

    request = (request_t*)malloc(sizeof(request_t)); /*Aloccate memory.*/
    token = strtok(buffer, "="); /*Split message*/
    value = strtok(NULL, "="); /*Extract Value*/
    request->ID = atoi(&(token[strlen(token) - 1])); /*Set up registers ID*/

    if (value == NULL) { /*Check value*/
        request->operation = READ;

    } else if (value != NULL) {
        if (strcmp(value, "?") == 0) {
            request->operation = INFO;
        } else {
            if (is_valid(value, request)) { /*Check if the  number is valid according to registers informations.*/
                request->operation = INSERT;
                item_counter = request->new_item[request->ID]; /*Initialize the operation*/
                regs[request->ID][item_counter] = atoi(value); /*Store the value into valid regs data according to ID and counters.*/
                item_counter++; /*Bacause of insert operation push up the right counter according to register's ID.*/
            } else
                request->operation = ERROR;
        }
    } else {
        free(request);
        return (NULL);
    }

    printf("Operation : %d\n", request->operation);

    return request;
}
/**
*@name process_request : Process a client request. Manage valid or invalid informations and send response to client via serial port. 
*@param request        : Informations about register
*@param fd             : The accept file descriptor
*@retval               : Nothing 
*/
void process_request(request_t* request, int fd)
{

    char response_str[RESPONSE_SIZE]; /* Buffer which will be send to client when a response is ready to be send.*/
    int item_counter;

    memset(response_str, 0, sizeof(response_str)); /*Clean buffer*/

    switch (request->operation) {
    case INFO:
        write(fd, informations[request->ID], sizeof(informations[request->ID]));
        break;
    case READ:
        item_counter = request->new_item[request->ID];
        sprintf(response_str, "%d", regs[request->ID][item_counter]);
        write(fd, response_str, sizeof(response_str));
        break;
    case INSERT:
        write(fd, "OK\n", 3);
        memset(response_str, 0, sizeof(response_str));
        break;
    case ERROR:
        write(fd, "INVALIDINPUT\n", 14);
        memset(response_str, 0, sizeof(response_str));
        break;
    }
}
int main(int argc, char* argv[])
{

    int fd, num_bytes = 0;
    char copy_msg[10];
    request_t* request;

    request = (request_t*)malloc(sizeof(request_t));

    printf("\033[0;31m ");

    if (argc < 1) {
        printf("You have to enter a serial port!\n");
        exit(1);
    }

    fd = open(argv[1], O_RDWR | O_NOCTTY | O_SYNC); /*Opening port */

    if (fd == -1) {
        perror(argv[1]);
        exit(1);
    }
    printf("[+] port opened successfully!\n");

    set_attributes(fd, B115200, 0); /*Setting Terminal attributes - Baud Rate and parity bit.*/

    start_up(request);

    receive_message = (char*)malloc(SIZE * sizeof(char)); /*Allocate memory for buffer*/

    memset(receive_message, 0, sizeof(receive_message)); /*Clean buffer*/

    while (1) {
        set_block(fd, 1);

        num_bytes = read(fd, receive_message, 20);
        if (num_bytes < 0)
            perror("Error in reading\n");
        else {
            if (strncmp(receive_message, "import.", 7) == 0) {
                strcpy(copy_msg, receive_message);
                initialize(copy_msg);

            } else {
                printf("message received : %s\n", receive_message);
                request = parse_request(receive_message);
                process_request(request, fd);
            }
        }
    }  
    close(fd); /* Close serial port*/

    for (int i = 0; i < 2; i++) /*free registers memory.*/
        free(regs[i]);
    free(regs);
}
