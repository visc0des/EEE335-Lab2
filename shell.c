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
       
        - Unfortunately, can't make the assumption of which directory serves as the home directory. 
        Don't have the tools to do that just yet. We'll have to stick to simply using the entire absolute 
        directory when switching directories and all. 


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
void fillNull(char **argv);

// --- Functions for system calls ---
void printPWD();
int listFiles();


int main() {

    char input[cmd_max_len];        // The input
    char *argv[max_num_arg];        // The arguments

    

    int pid, status;

    welcome_prompt();

    while (1) {

        // First thing every loop, fill up argv with null chars
        fillNull(argv);

        // Get input
        type_prompt(input);

        // Parse the tokens. For now, print parsed tokens
        int arrLen = parse(input, argv);

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
    Description:
    
        This function parses the user inputs.   
 
    Returns: number of arguments parsed <-- this was added for debugging purposes. 
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
 *  Description: 
 *
 *      This function interprets the parsed command that is entered by the user and
 *      calls the appropriate built-in function or calls the appropriate program.
 * 
 *      Commands exit, ls, pwd, and cd have been implemented with system calls. 
 *      All remaining valid commands utilize execvp(). 
 * 
 *  Arguments: 
 *      char** argv: an array of strings. First element contains the parsed command token, any other
 *          following tokens are the command's arguments.  
 */
void interpret_command(char **argv) {

    // TODO - handle the bad input for each command
    // Retrieve the command token from argv
    char* first_token = argv[0];

	// Command token determines which system call/built-in command to execute. 
    if (strcmp(first_token, "exit") == 0) {

        // Handle bad input, 
        if (argv[1] != NULL) {
            fprintf(stderr, "\n\nERROR: exit command not allowed arguments.");
            return;
        }

        // Call exit() system call
        printf("\nExiting from the process...\n\n");
        sleep(2); // Not necessary, but adds a bit of flare.
        exit(0);

        
    }
    else if (strcmp(first_token, "cd") == 0){

        // Handle bad input
        if (argv[2] != NULL) {
            fprintf(stderr, "\n\nERROR: cd command only allowed one argument.");
            return;
        }


        // Change the directory with chdir();
        int result = chdir(argv[1]);
        if (result == 0) {
            printf("\n\nWorking directory successfully changed.");
        }
        else {
            fprintf(stderr, "\n\nERROR: Inputted directory not recognized. ");
        }


    }
    else if (strcmp(first_token, "ls") == 0){

        // Handle bad input
        if (argv[1] != NULL) {
            fprintf(stderr, "\n\nERROR: ls command not allowed arguments (this implementation of it). ");
            return;
        }
   
        // List all files in current working directory 
        listFiles();
        
    }
    else if (strcmp(first_token, "pwd") == 0) {

        // Handle bad input
        if (argv[1] != NULL) {
            fprintf(stderr, "\n\nERROR: pwd command not allowed arguments. ");
            return;
        }

        // Print current working directory
        printPWD();

    }
    else {


        // Fork the current process and make a child, capture result
        int result = fork();

        // Make parent process wait for its child to finish
        if (result != 0) {

            // Going into blocked state...
            int status;
            waitpid(-1, &status, 0);

        }
        // Make child run the typed in command. 
        else {
            
            int n = execvp(first_token, argv);

            // If inputted command not recognized, kill this child process
            if (n == -1) {
                fprintf(stderr, "\n\nERROR: Command <%s> failed to execute. It or its attributes not recognized.\n", first_token);
                exit(-1);
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


/*  
    Description:

        Functions prints the absolute path of current working directory to console. 
        Prints out to console if an error had occurred. 

 */
void printPWD() {

    // dynamic memoorrryyy...
    char* cwd;
    cwd = getcwd(NULL, 0);
    if (cwd != NULL) {
        printf("\nCurrent working directory: %s", cwd);
    }
    else {
        fprintf(stderr, "\n\nERROR: Could not open current working directory.");
    }
    free (cwd);

}


/*  Description:

        Function lists all non-hidden files/directories in current working directory.
        Utilizes directory entities and directory-related system calls. 

    Returns: 0 if successful. -1 if unsuccessful. 

 */
int listFiles() {

    // First, open current directory and capture with pointer. Check if successful
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

            // A little extra stuff...
            char* dir_suffix;
            if (this_ent -> d_type == DT_DIR) { dir_suffix = " - directory"; } else { dir_suffix = ""; }

            printf("\n%s%s", this_ent -> d_name, dir_suffix);
        }

        // Get next entity
        this_ent = readdir(curr_dir);

    }

    // Finish by closing directory pointer
    closedir(curr_dir);
    return 0;

}


/* 
    Description:

        Prints all parsed tokens. 
    
    Arguments: 
        char **argv : The string array which contains parsed tokens.
        int arrLen: The length of argv. 

*/
void printTok(char **argv, int arrLen) {

    for (int i = 0; i < arrLen; i++) {
        printf("\nToken %d: %s", i, argv[i]);
    }
}


/* Description:

        Function fills up the inputted array with null characters. 

    Arguments:
        char** inArr: the array of characters (of size max_num_arg) to be filled up with null characters. 

*/
void fillNull(char** inArr) {
    for(int i = 0; i < max_num_arg; i++) {
            inArr[i] = '\0';
        }
}