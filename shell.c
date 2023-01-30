#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


const unsigned int cmd_max_len = 1024;
const unsigned int max_num_arg = 64;


void welcome_prompt();
void parse(char *input, char **argv);
void type_prompt(char *input);
void interpret_command(char **argv);


int main() {

    char input[cmd_max_len];        // The input
    char *argv[max_num_arg];        // The arguments

    int pid, status;

    welcome_prompt();

    while (1) {
        type_prompt(input);
        parse(input, argv);
        interpret_command(argv);
    }

    return 0;
}


/*
 * This functions prints the prompt message and accepts input from the user.
 */
void type_prompt(char *input) {

    printf("EEE335$ ");

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
 * This function parses the user inputs.
 */
void parse(char *input, char **argv) {

    // This is where the code for parsing the user's input is.

}


/*
 * This function interprets the parsed command that is entered by the user and
 * calls the appropriate built-in function or calls the appropriate program.
 */
void interpret_command(char **argv) {

	// This is where you will write code to call the appropriate function or program.

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
