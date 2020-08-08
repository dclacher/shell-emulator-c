#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>

/* A function that checks if the user has entered "exit" as the input */
int compareStringToExit(char *str1);
/* A function to isolate the code that executes a command provided by the user */
void executeCommand(char *command);

int main(int argc, char *argv[])
{
    int status;
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
        // get the other commands, if there is at least one more
        while (token != NULL) {
            int w;
            for (w = 0; w < 64 * sizeof(char); w++)
            {
                tempToken[w] = token[w];
            }
            //tempToken[w + 1] = ';';
            // Fork
            int pid = fork();
            if (pid == -1)
            {
                perror("impossible to fork");
                exit(1);
            }
            if (pid > 0) // Parent
            {
                if (wait(&status) >= 0 && WIFEXITED(status))
                {
                    // the "pid" variable here is the pid of the child returned to the parent
                    // the "write" function here should get an array if integers
                    write(STDOUT_FILENO, &status, sizeof(int));
                    write(STDOUT_FILENO, &pid, sizeof(int));
                    //exit(12);
                }
                //free(stdinBuffer);
                //free(token);
                //free(tempToken);
                token = strtok(NULL, delimiter);
            }
            else if (pid == 0) // Child
            {
                executeCommand(tempToken);
            }
        }
        /* TODO The CTRL+D part isn't working
        if (stdinBuffer[0] == '\004')
        {
            write(STDOUT_FILENO, byeMessage, sizeof(byeMessage));
            exit(1);
        }*/
    }
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
        tk = strtok(NULL, space);
        argv[i] = tk;
        i++;
    }
    argv[i] = NULL;
    execvp(argv[0], argv);
    exit(9);
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
