#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>

/*Translate the pseudo-code below into a C program to act like your own basicl shell that pro- cesses single or multiple commands entered
by the user. In particular, the program assembles and executes each command (do not implement piping).
The commands/programs location can be anywhere in $PATH and might have arguments.
Examples of input lines entered by the user :
    ls -F
    ls -F; date ; echo Hello
In particular,
    • Your program should assemble and execute each command from the input line. 
   • You should only use Unix I/O system calls.
    • You can run the provided executable for a demonstration.
*/

/* A function that checks if the user has entered "exit" as the input */
int compareStringToExit(char *str1);
/* A function to isolate the code that executes a command provided by the user */
void executeCommand(char *command);

int main(int argc, char *argv[])
{
    char *stdinBuffer, *token, *tempToken;
    const char byeMessage[] = "Bye bye.\n";
    const char delimiter[2] = ";";

    while (1)
    {
        stdinBuffer = malloc(1024 * sizeof(char));
        token = malloc(64 * sizeof(char));
        tempToken = malloc(64 * sizeof(char));
        // reading command line from the user
        read(STDIN_FILENO, stdinBuffer, 1024);
        // if the input is "exit" or CTR+D, print "Bye bye" and terminate the shell
        if (!compareStringToExit(stdinBuffer))
        {
            write(STDOUT_FILENO, byeMessage, sizeof(byeMessage));
            exit(1);
        }
        // get the first command
        token = strtok(stdinBuffer, delimiter);
        printf("first token in main: %s\n", token);
        // get the other commands, if there is at least one more
        while (token != NULL) {
            for (int w = 0; w < 64 * sizeof(char); w++)
            {
                tempToken[w] = token[w];
            }
            printf("token inside while in main: %s\n", token);
            printf("temp token inside while in main: %s\n", tempToken);
            token = strtok(NULL, delimiter);
            printf("token inside while in main after calling strtok again: %s\n", token);
            printf("temp token inside while in main after calling strtok again: %s\n", tempToken);
            executeCommand(tempToken);
        }
        /* TODO The CTRL+D part isn't working
        if (stdinBuffer[0] == '\004')
        {
            write(STDOUT_FILENO, byeMessage, sizeof(byeMessage));
            exit(1);
        }*/
        free(stdinBuffer);
        free(token);
        //free(tempToken);
    }
    
    /*While(1)
        read command line from user // Use Unix read/write system calls 
       if ( line == "exit" or CTR-D )
            - print "Bye Bye" and terminate shell
        for each command
            - assemble command args in a vector // you can use strtok() as a helper
            - duplicate current process (fork)
            - child should exec to the new program
            - parent process waits for its child to terminate
            - parent prints a message with pid and returned status of finished child process*/
}

void executeCommand(char *command)
{
    char *tk;
    char *argv[10];
    const char space[2] = " ";
    tk = strtok(command, space);
    argv[0] = tk;
    int i = 1;
    while (tk != NULL) {
        printf("token inside while in executeCommand: %s\n", tk);
        tk = strtok(NULL, space);
        argv[i] = tk;
        i++;
    }
    for (int x = 0; x < 3; x++)
    {
        printf("loop to print argv: %s\n", argv[x]);
    }
}

int compareStringToExit(char *str1)
{
   int c = 0;
   char *str2;
    str2 = malloc(1024 * sizeof(char));
    str2[0] = 'e';
    str2[1] = 'x';
    str2[2] = 'i';
    str2[3] = 't';
    str2[4] = '\n';
 
   while (str1[c] == str2[c]) {
      if (str1[c] == '\0' || str2[c] == '\0')
         break;
      c++;
   }
   
   if (str1[c] == '\0' && str2[c] == '\0')
      return 0;
   else
      return -1;
}
