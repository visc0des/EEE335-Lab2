/*
    Authors: NCdt Eric Cho and OCdt Liethan Velasco

    Description: 

        This file contains the code pertaining to Lab 2 of the course EEE335.
        This code is to create a very rudimentary version of a Linux shell. 

    Version: January 30th 2023

    Status:
        - Implementing parser for input. 


    NOTES:
        - TEMPORARY: modified parse() to return the token count parsed from input. 
        - added printArgs() function to print parsed input.

*/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

// ---- For system calls ----
#include <unistd.h> // For getcwd()


const unsigned int cmd_max_len = 1024;
const unsigned int max_num_arg = 64;


void welcome_prompt();
int parse(char *input, char **argv);
void printTok(char **argv, int arrLen);
void type_prompt(char *input);
void interpret_command(char **argv);


int main() {

    char input[cmd_max_len];        // The input
    char *argv[max_num_arg];        // The arguments

    

    int pid, status;

    welcome_prompt();

    while (1) {

        for(int i = 0; i < max_num_arg; i++)        {
        argv[i] = '\0';
        }

        // Get input
        type_prompt(input);

        // Parse the tokens. For now, print parsed tokens
        int arrLen = parse(input, argv);
        printTok(argv, arrLen);

        // Interpret command. 
        interpret_command(argv);

    }

    return 0;
}


/*
 * This functions prints the prompt message and accepts input from the user.
 */
void type_prompt(char *input) {

    printf("\nEEE335:~$ ");

    if (input != NULL) {
        int c = EOF;
        int i = 0;

        // Take in input until user hits enter or end of file is encountered.
        while ((c = getchar()) != '\n' && c != EOF) {
            input[i++] = (char) c;
        }

        input[i] = '\0';
    }
}


/*
    This function parses the user inputs.   
 
    RETURNS number of arguments parsed. TODO - REMOVE RETURN 
 */
int parse(char *input, char **argv) {

    // Defining separator
    char sep[2] = " ";

    // Put first token of input into argv array
    char* token = strtok(input, sep);
    argv[0] = token;

    // Creating counter to return parsed token count
    int tokCount = 1;

    // Parse the remaining input string, keeping in max_num_arg range...
    for (int i = 1; i < max_num_arg; i++) {

        // Retrieve the next token. If null (IOW no tokens left), stop loop.
        token = strtok(NULL, sep);
        if (token == NULL) { break; }

        // Otherwise, store collected token and increment argCount
        argv[i] = token;
        tokCount++;

        // Debug statement
        printf("\n Adding token <%s> to index %d. Token count = %d", token, i, tokCount);
        
    }

    return tokCount;

}

/* 

Prints all parsed tokens. 

char **argv : The string array which contains parsed tokens.
int arrLen: The length of argv. 

*/
void printTok(char **argv, int arrLen) {

    printf("\n\nIn printArgs() function. \n");

    for (int i = 0; i < arrLen; i++) {
        printf("\nToken %d: %s", i, argv[i]);
    }
}


/*
 * This function interprets the parsed command that is entered by the user and
 * calls the appropriate built-in function or calls the appropriate program.
 */
void interpret_command(char **argv) {
    printf("In interpret_command() function.\n");
    char* first_token = argv[0];
    printf("%s", first_token);
	// This is where you will write code to call the appropriate function or program.
    if (strcmp(first_token, "exit") == 0){
        printf("First token: %s", first_token);
    }
    else if (strcmp(first_token, "cd") == 0){
     printf("First token: %s", first_token);
    }
    else if (strcmp(first_token, "ls") == 0){
    printf("First token: %s", first_token);
    }
    else if (strcmp(first_token, "pwd") == 0){

        printf("First token: %s", first_token);
       
        // Print current working directory with dynamic memory
        char* cwd;
        cwd = getcwd(NULL, 0);
        if (cwd != NULL) {
            printf("\n\nCurrent working directory> %s", cwd);
        }
        else {
            fprintf(stderr, "\n\nERROR: could not find current working directory.");
        }


    }
    else{
        printf("Not a builtin");


    //  create child process with fork
    if(fork() == 0){
        int n = execvp (first_token, argv);
        if(n == -1){
            printf("Command failed to execute.\n");
        }
    }
    }
}


/*
 * This function prints the welcome message.
 */
void welcome_prompt() {
    int num_Padding = 41;
    int i;

    // Prints the first line of padding.
    for (i = 0; i < num_Padding; i++)
        printf("#");
    printf("\n#\tWelcome to the EEE335 shell\t#\n");
    for (i = 0; i < num_Padding; i++)
        printf("#");
    printf("\n\n");
}
