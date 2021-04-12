#include "toptions/term_options.h" /*Header file full location*/
#include <errno.h>
#include <fcntl.h> /*file control*/
#include <stdio.h> /*standard input output*/
#include <stdlib.h>
#include <string.h> /*string functions*/
#include <sys/types.h>
#include <termios.h> /*posix terminal*/
#include <unistd.h>

#define SIZE 100
#define MESSAGE_SIZE 256

typedef struct informations {
    char name[10];
    char num_limits[10];
    int ID;
} informations_t;

informations_t* insert_reg;
char message_validation[50];
int last_index = 0; /* Searches infos array for last index after users imports new register */
int count_registers = 0;
int i = 0;
char* exist_regs[SIZE];
char* infos[SIZE] = { "\tAvailable AT  commands",
    "'REG1': Read the register-1 value -> Responce:'int'",
    "'REG1=?': Read the list of all allowed values for register-1 (allows 2bytes  integers)->Responce:'0-16535'",
    "'REG1=<int>' :Write the provided integer to register-1 Responce :'OK|InvalidInput",
    "'REG2':Read the register-2 value ->Responce:'<int>'",
    "'REG2=?':Read the list of all allowed values for register-2 ->Responce:'1|2|3'",
    "'REG2=<int>' :Write the provided integer to register-2 Responce :'OK|InvalidInput" };
/*
*@name is_inserted : Detects whether the registry the user wants to import has already been imported or not.
*@param name       : Points to register's name
*@return           : "1" if is already inserted otherwise "0".
*/
int is_inserted(char* name)
{

    for (int i = 1; i < SIZE; i++) {

        if (exist_regs[i] != NULL) {
            if (strcmp(name, exist_regs[i]) == 0) {
                return 1;
            }
        }
    }
    return 0;
}
/*
*@name help_function   : Help User with AT-COMMANDS and give him a chance to create new registers
*@param fd             : The accept file descriptor , in order to send proper message to server if user wants to insert new register.
*@return    		   : Nothing
*/
void help_function(int fd)
{
    char choice; /*User choice, in case of insert or not */
    char *buf1, *buf2, *buf3, *buf4, *join; /* Help bufs for creating strings ,(1-3) in order to add them in infos array and (4-join) to create import.<string>.*/
    int check_insert = 0; /* If check_insert becomes "1" then the register that user wants to insert is already inserted.*/
    char *split, *id, cpy_msg[10];

    char reg_name[10];
    /*Initialize array*/
    insert_reg = (informations_t*)malloc(sizeof(informations_t));

    buf1 = (char*)malloc(100 * sizeof(char));
    buf2 = (char*)malloc(100 * sizeof(char));
    buf3 = (char*)malloc(100 * sizeof(char));
    buf4 = (char*)malloc(100 * sizeof(char));
    join = (char*)malloc(100 * sizeof(char));

    /* Pritnts out HELP commands for user and calculates last index from infos array */
    for (int i = 0; i < SIZE; i++) {
        if (infos[i] != NULL) {
            printf("%s\n", infos[i]);
            last_index++;
        }
    }

    printf("-----------------------------------IMPORT-----------------------------------\n");
    printf("import(y/n) # ");
    scanf("%s", &choice);
    if (strcmp(&choice, "y") == 0 || strcmp(&choice, "Y") == 0) {
        printf("REGS name :");
        scanf("%s", insert_reg->name);

        sprintf(cpy_msg, "%s", insert_reg->name);
        split = strtok(cpy_msg, "G");
        id = strtok(NULL, "G");
        insert_reg->ID = atoi(id);

        check_insert = is_inserted(insert_reg->name);
        if (check_insert == 0) { /* If no register with that name exists in "data-base" insert then new one*/

            count_registers++; /* Means that new register is gonna be inserted , so make new room for it by increasing counter. */
            printf("**NOTE->In case of  a) distinct numbers place  '|' between them otherwise b) with '-' ,example : a)1|2|3 or b)0-100.\n");
            printf("Bounds :");
            scanf("%s", insert_reg->num_limits);
            printf("User inputs regs name as : %s and number limit as : %s\n", insert_reg->name, insert_reg->num_limits);
            /* Create proper strings in order to include them into informations array (info).*/
            sprintf(buf1, "'%s':Read the register-%d value -> Response:'int'", insert_reg->name, insert_reg->ID);
            sprintf(buf2, "'%s=?':Read the list of all allowed values for register-%d (allows 2bytes  integers)->Response:'%s'", insert_reg->name, insert_reg->ID, insert_reg->num_limits);
            sprintf(buf3, "'%s=<int>':Write the provided integer to register-%d Response:'OK|InvalidInput'", insert_reg->name, insert_reg->ID);
            /* Fix  bufs in order to create import message for server. */
            sprintf(buf4, "%s", insert_reg->num_limits);
            sprintf(join, "%s", "import.");
            /* Initialize new informations for user.*/
            infos[last_index + 1] = buf1;
            infos[last_index + 2] = buf2;
            infos[last_index + 3] = buf3;
            exist_regs[count_registers] = insert_reg->name; /* Insert new register in "data-base". */
            strcat(join, buf4); /* Create message import.<string> in order to inform server that user wants to add new register. */
            write(fd, join, 20); /* Send message to server. */
            i++;
        } else {
            printf("This Register is already inserted.\n");
            printf("Continue Process.\n");
        }
    } else {
        printf("Continue Process.\n");
    }
}
/*
*@name isValid : Parses the message of user and determines if it is an AT command
*@param  msg   : User message 
*@return       : 1 or 0.
*/
int is_valid(char* msg)
{
    char *token = NULL, *value = NULL, *token2 = NULL, *REG = NULL;
    int msg_len; /* Length of message */

    msg_len = strlen(msg);
    if (msg_len < 7 || msg_len > 12) {
        return 0;
    } else {
        token = strtok(msg, "=");
        value = strtok(NULL, "=");
        token2 = strtok(token, "+");
        REG = strtok(NULL, "+"); /*Extract register's name.*/

        if (is_inserted(REG) == 0) { /* If register is not inserted yet return 0.*/
            printf("You need to insert %s in order to use it.\n", REG);
            return 0;

        } else {
            return 1;
        }
    }
}
/*
*@name write_message : Writting user's message to servers and waits for response.
*@param 	fd		  : The accept file descriptor
*@param 	msg		  : User message
*@return 			  : Nothing
*/
void write_message(int fd, char* msg)
{

    int rd;
    char response[MESSAGE_SIZE]; /* Response message from server.*/

    write(fd, msg, 30);
    set_block(fd, 0);
    rd = read(fd, response, sizeof(response));
    if (rd) {

        printf("Response:"); /*Got response from server*/
        printf("%s\n", response);
        memset(response, 0, sizeof(response));
    }
}
int main(int argc, char* argv[])
{

    int fd;
    char user_msg[MESSAGE_SIZE];
    struct termios tty;

    printf("\033[0;36m");

    if (argc < 1) {
        printf("You have to enter a serial port!\n");
        exit(1);
    }

    fd = open(argv[1], O_RDWR | O_NOCTTY); /*Openning Port*/
    if (fd < 0) {
        perror(argv[1]);
        exit(1);
    }
    printf("[+] port is opended succussfully\n");

    set_attributes(fd, B115200, 0); /*Setting Terminal attributes - Baud Rate and parity bit*/
    exist_regs[1] = "REG1";
    exist_regs[2] = "REG2";
    while (1) {
        printf(">");
        scanf("%s", user_msg);
        strcpy(message_validation, user_msg);
        if (strcmp(user_msg, "help") == 0 || strcmp(user_msg, "HELP") == 0)
            help_function(fd);
        else {
            if (is_valid(message_validation) == 1) { /* Message Validation Check*/
                write_message(fd, user_msg);
            } else {
                printf("This is not an AT command , try again!\n");
            }
        }
    }
    close(fd); //Close serial port
}
