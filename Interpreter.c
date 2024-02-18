//Шарма Рохит
//15.11Д-МОСИП11/21Б
//Вариант №9

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int shell_active = 1; // Variable to maintain shell activity, used for the 'exit' command.

// Commands are defined as macros for ease of functionality extension.
#define SHCMD(x) int shcmd_##x (char* cmd, char* params[])
#define SHCMD_EXEC(x) shcmd_##x (params[0], params)
#define IS_CMD(x) strncmp(#x,cmd,strlen(cmd)) == 0

// Shell built-in commands: pwd, exit, and others.
SHCMD(pwd)
{
    printf("%s\n",getenv("PWD"));
    return 0;
}

// wc command for counting lines, words, and characters in a file.
// Checks for arguments before execution.
// Example usage: wc <filename>
// Followed by the logic of the wc command.
SHCMD(wc)
{
    if (params[1] == NULL)
    {
        fprintf(stderr, "Usage: wc <filename>\n"); // Outputs the usage message for the wc command to the standard error stream.
        return 1;
    }

    FILE *file = fopen(params[1], "r"); // Open the file for reading.

    if (file == NULL)// If the file is not opened, output an error message.
    {
        perror("fopen");
        return 1;
    }

    int lines = 0, words = 0, chars = 0; // Initialize variables for counting lines, words, and characters.
    char buffer[1024]; // Buffer for reading the file.
    while (fgets(buffer, sizeof(buffer), file)) // Loop for reading the file.
    {
        chars += strlen(buffer); // Count the number of characters in the line.
        lines++;
        char *token = strtok(buffer, " \t\n"); // Count the number of words in the line.
        while (token) // Count the number of words in the line.
        {
            words++;
            token = strtok(NULL, " \t\n"); // Count the number of words in the line.
        }
    }

    printf("%d %d %d %s\n", lines, words, chars, params[1]); // Output the number of lines, words, and characters in the file.

    fclose(file); // Close the file.
    return 0;
}

// cat command for displaying the contents of a file.
// Example usage: cat <filename>
// Followed by the logic of the cat command.
SHCMD(cat)
{
    if (params[1] == NULL)
    {
        fprintf(stderr, "Usage: cat <filename>\n"); // Outputs the usage message for the cat command to the standard error stream.
        return 1;
    }

    FILE *file = fopen(params[1], "r"); // Open the file for reading.
    if (file == NULL)
    {
        perror("fopen"); // Output an error message.
        return 1;
    }

    char line[1024]; // Buffer for reading the file.
    while (fgets(line, sizeof(line), file)) // Loop for reading the file.
    {
        printf("%s", line); // Display the contents of the file.
    }

    fclose(file);
    return 0;
}

// grep command for searching lines containing a specific pattern in a file.
// Example usage: grep <pattern> <filename>
// Followed by the logic of searching for lines in the file matching the specified pattern.
SHCMD(grep)
{
    if (params[1] == NULL || params[2] == NULL)
    {
        fprintf(stderr, "Usage: grep <pattern> <filename>\n"); // Outputs the usage message for the grep command to the standard error stream.
        return 1;
    }

    FILE *file = fopen(params[2], "r"); // Open the file for reading.
    if (file == NULL)
    {
        perror("fopen"); // Output an error message.
        return 1;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, params[1]) != NULL) // If the line contains the pattern, display it.
        {
            printf("%s", line); // Display the contents of the file.
        }
    }

    fclose(file);
    return 0;
}

// link command for creating a hard link to a file.
// Example usage: link <sourcefile> <targetfile>
// Followed by the logic of creating a hard link using the link() system call.
SHCMD(link)
{
    if (params[1] == NULL || params[2] == NULL)
    {
        fprintf(stderr, "Usage: link <sourcefile> <targetfile>\n"); // Outputs the usage message for the link command to the standard error stream.
        return 1;
    }

    if (link(params[1], params[2]) == -1) // Call the link() function to create a hard link.
    {
        perror("link");
        return 1;
    }

    return 0;
}

// nl command for displaying the contents of a file with line numbers.
// Example usage: nl <filename>
// Followed by the logic of displaying file lines with added line numbers.
SHCMD(nl)
{
    if (params[1] == NULL)
    {
        fprintf(stderr, "Usage: nl <filename>\n"); // Outputs the usage message for the nl command to the standard error stream. stderr - standard error stream.
        return 1;
    }

    FILE *file = fopen(params[1], "r");

    if (file == NULL)
    {
        perror("fopen"); // Output an error message.
        return 1;
    }

    char line[1024];
    int line_number = 1;
    while (fgets(line, sizeof(line), file)) // sizeof - number of characters in a line.
    {
        printf("%6d  %s", line_number++, line); // Display the contents of the file with added line numbers.
    }

    fclose(file);
    return 0;
}
// exit command to exit the shell.
// Example usage: exit
// Sets the value of the shell_active variable to 0, leading to the termination of the loop in main() and exiting the program.
SHCMD(exit)
{
    shell_active = 0;
    printf("Good Bye ;)\n");
    return 0;
}

// Function for executing commands with parameters.
void my_exec(char *cmd)
{
    char *params[256]; // Array for storing parameters of the command separated by spaces.
    char *token;
    int np;
    // Splitting the input string into command and parameters.
    // Followed by command checking and execution.
    token = strtok(cmd, " "); // Split the input string into command.
    np = 0; // Initialize the variable to store parameters of the command separated by spaces.

    while( token && np < 255 ) // Loop to split the input string into command and parameters.
    {
        params[np++] = token; // Record parameters of the command separated by spaces into the array.
        token = strtok(NULL, " "); // Split the input string into command.
    }
    params[np] = NULL;

    if( IS_CMD(pwd) )
        SHCMD_EXEC(pwd);
    else if( IS_CMD(exit) )
        SHCMD_EXEC(exit);
    else if( IS_CMD(cat) )
        SHCMD_EXEC(cat);
    else if( IS_CMD(wc) )
        SHCMD_EXEC(wc);
    else if( IS_CMD(nl) )
        SHCMD_EXEC(nl);
    else if( IS_CMD(link) )
        SHCMD_EXEC(link);
    else if( IS_CMD(grep) ) // Added check for the grep command
        SHCMD_EXEC(grep);
    else
    { // Otherwise, execute an external command
        execvp(params[0], params);
        perror("exec"); // If there is an error during execution
    }
}

// Recursive function for handling pipelines
// Parameters: command string, number of commands in the pipeline, current command index (starts from 0)
int exec_conv(char *cmds[], int n, int curr)
{
    int fd[2],i;

    // Creating channels for data transfer between commands in the pipeline.
    // Recursive call to the function to handle each command in the pipeline.
    // Followed by the logic of handling the pipeline.
    if( pipe(fd) < 0 )
    {
        printf("Cannot create pipe\n");
        return 1;
    }

    if( n > 1 && curr < n - 2 )
    { // first n-2 cmds
        if( fork() == 0 )
        {
            dup2(fd[1], 1);
            close(fd[0]);
            close(fd[1]);
            my_exec(cmds[curr]);
            exit(0);
        }
        if( fork() == 0 )
        {
            dup2(fd[0], 0);
            close(fd[0]);
            close(fd[1]);
            exec_conv(cmds,n,++curr);
            exit(0);
        }
    }
    else
    { // 2 last cmds or if only 1 cmd
        if( n == 1 && strcmp(cmds[0],"exit") == 0 )
        { // For the case of the exit command, we bypass without fork; otherwise, we won't be able to terminate.
            close(fd[0]);
            close(fd[1]);
            my_exec(cmds[curr]);
            return 0;
        }
        if( fork() == 0 )
        {
            if( n > 1 ) // if more than 1 cmd
                dup2(fd[1], 1);
            close(fd[0]);
            close(fd[1]);
            my_exec(cmds[curr]);
            exit(0);
        }
        if( n > 1 && fork()==0 )
        {
            dup2(fd[0], 0);
            close(fd[0]);
            close(fd[1]);
            my_exec(cmds[curr+1]);
            exit(0);
        }
    }
    close(fd[0]);
    close(fd[1]);

    for( i = 0 ; i < n ; i ++ ) // waiting for the termination of all child processes
        wait(0);

    return 0;
}

// Main function, loop for input strings (splitting pipeline, executing command)
int main()
{
    char cmdline[1024]; // Buffer for storing input strings.
    char *p, *cmds[256], *token;
    int cmd_cnt;

    // Main shell loop for input and command processing.
    // Splits the input string into individual commands separated by '|'.
    // Then calls the exec_conv function for their execution.
    while( shell_active )
    {
        printf("[%s]# ",getenv("PWD"));
        fflush(stdout);

        fgets(cmdline,1024,stdin);
        if( (p = strstr(cmdline,"\n")) != NULL ) p[0] = 0;

        token = strtok(cmdline, "|");
        for(cmd_cnt = 0; token && cmd_cnt < 256; cmd_cnt++ )
        {
            cmds[cmd_cnt] = strdup(token);
            token = strtok(NULL, "|");
        }
        cmds[cmd_cnt] = NULL;

        if( cmd_cnt > 0 )
        {
            exec_conv(cmds,cmd_cnt,0);
        }
    }

    return 0;
}
