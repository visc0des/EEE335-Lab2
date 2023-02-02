/*
    Authors: NCdt Eric Cho and OCdt Liethan Velasco

    Description: 

        This file contains the code pertaining to Lab 2 of the course EEE335.
        This code is to create a very rudimentary version of a Linux shell. 

    Version: January 30th 2023

    Status:
        - Implementing built-in exit. 


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
#include <unistd.h> // For getcwd() and chdir() <-- not sure about last one
#include <dirent.h> // For opendir() 



const unsigned int cmd_max_len = 1024;
const unsigned int max_num_arg = 64;


void welcome_prompt();
int parse(char *input, char **argv);
void printTok(char **argv, int arrLen);
void type_prompt(char *input);
void interpret_command(char **argv);

// --- Functions for system calls ---
void printPWD();
int listFiles();


int main() {

    char input[cmd_max_len];        // The input
    char *argv[max_num_arg];        // The arguments

    

    int pid, status;

    welcome_prompt();

    while (1) {


        for(int i = 0; i < max_num_arg; i++) {
            argv[i] = '\0';
        }

        // Get input
        type_prompt(input);

        // Parse the tokens. For now, print parsed tokens
        int arrLen = parse(input, argv);
        // printTok(argv, arrLen);

        // Interpret command. 
        interpret_command(argv);

    }

    return 0;
}


/*
 * This functions prints the prompt message and accepts input from the user.
 */
void type_prompt(char *input) {

    // Get current working directory to include in prompt (absolute for now) TRUNCATE?
    char* abs_cwd = getcwd(NULL, 0);
    printf("\n\nEEE335 > %s:~$ ", abs_cwd);
    free(abs_cwd);

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

        
    }

    return tokCount;

}

/* 

Prints all parsed tokens. 

char **argv : The string array which contains parsed tokens.
int arrLen: The length of argv. 

*/
void printTok(char **argv, int arrLen) {

    for (int i = 0; i < arrLen; i++) {
        printf("\nToken %d: %s", i, argv[i]);
    }
}


/*
 * This function interprets the parsed command that is entered by the user and
 * calls the appropriate built-in function or calls the appropriate program.
 */
void interpret_command(char **argv) {

    // TODO - handle the bad input for each command

    char* first_token = argv[0];

	// This is where you will write code to call the appropriate function or program.
    if (strcmp(first_token, "exit") == 0){

        // Call exit() system call
        printf("\nExiting from the process...\n\n");
        sleep(2); // Not necessary, but adds a bit of flare.
        exit(0);

        
    }
    else if (strcmp(first_token, "cd") == 0){

        // Can't allow if more than one arg passed
        if (argv[2] != NULL) {
            fprintf(stderr, "ERROR: cd command only allowed one argument.");
            return;
        }


        // Change the directory
        int result = chdir(argv[1]);
        if (result == 0) {
            printf("\n\nWorking directory successfully changed.");
        }
        else {
            fprintf(stderr, "\n\nERROR: Inputted directory not recognized. ");
        }


    }
    else if (strcmp(first_token, "ls") == 0){

        // Check if any args passed. Stop if so
        if (argv[1] != NULL) {
            fprintf(stderr, "ERROR: current implementation of ls command does not allow arguments.");
            return;
        }
   
        // List all files in current working directory here
        listFiles();
        
    }
    else if (strcmp(first_token, "pwd") == 0){

        // Check if any args passed. Stop if so
        if (argv[1] != NULL) {
            fprintf(stderr, "ERROR: pwd command does not take any arguments.");
            return;
        }

        // Print current working directory
        printPWD();

        
    }
    else {

        // Commands for this include: cat, less, etc.

        // Fork the current process, capture result
        int result = fork();
        printf("\n\n Just forked... Now waiting... result = %d", result);

        // If we forked from parent, wait for child to finish
        if (result != 0) {

            printf("\n\n Parent > Waiting on child to finish...");

            // Going into blocked state...
            int status;
            waitpid(-1, &status, 0);

            printf("\n\n Parent > Child finished. Back in parent process. ");

        }
        // If we forked in child, do inputted command
        else {

            printf("\n\n Child > in the child process. Doing command..."); 

            sleep(1);
            
            int n = execvp(first_token, argv);

            // If command did not get recognized, kill the child process
            if (n == -1) {
                fprintf(stderr, "\n\nCommand %s failed to execute/not recognized.\n", first_token);
                exit(-1);
            }
        }

        


        /*
        //  create child process with fork
        if(fork() == 0){
            int n = execvp (first_token, argv);
            if(n == -1){
                printf("Command failed to execute.\n");
            }
        }
        */
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


/* Function prints current working directory. 
    Prints if error occurs.  */
void printPWD() {

    char* cwd;
    cwd= getcwd(NULL, 0);
    if (cwd != NULL) {
        printf("\nCurrent working directory: %s", cwd);
    }
    else {
        fprintf(stderr, "\n\nERROR: could not find current working directory.");
    }
    free (cwd);

}

/* Function lists all of the non-hidden files/directories in the current working directory.

Utilize directory entities. Returns 0 if successful, -1 if unsuccessful. */
int listFiles() {

    // First, open current directory and capture with pointer, and check if successful
    DIR* curr_dir = opendir(".");
    if (curr_dir == NULL) {
        fprintf(stderr, "\n\nERROR: Failed to open current directory.");
        return -1;
    }

    // Create a struct pointer to iterate through all files/dir in curr_dir. Initialize 
    // to point to first entity 
    struct dirent* this_ent;
    this_ent = readdir(curr_dir);

    // Iterate through all entities, print their names, until this_ent is NULL
    printf("\nContents of current working directory:\n");
    while (this_ent != NULL) {

        // Only print if ent name does not start with '.'
        char* ent_name = this_ent -> d_name;
        if (*ent_name != '.') {
            printf("\n%s", this_ent -> d_name);
        }

        // Get next entity
        this_ent = readdir(curr_dir);

    }

    // Finish by closing directory pointer
    closedir(curr_dir);
    return 0;

}
