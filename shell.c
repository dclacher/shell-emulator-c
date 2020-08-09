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
    char *stdinBuffer, *token;
    const char byeMessage[] = "Bye bye.\n";
    const char delimiter[4] = ";";
    char *vector[20];

    while (1)
    {
        stdinBuffer = malloc(1024 * sizeof(char));
        token = malloc(16 * sizeof(char));
        // reading command line from the user
        read(STDIN_FILENO, stdinBuffer, 1024);
        // if the input is "exit" or CTR+D, print "Bye bye" and terminate the shell
        if (!compareStringToExit(stdinBuffer))
        {
            write(STDOUT_FILENO, byeMessage, sizeof(byeMessage));
            exit(1);
        }
        // removing the line break that is added when the user presses ENTER in stdin
        stdinBuffer = strtok(stdinBuffer, "\n");
        /* TODO The CTRL+D part isn't working
        if (stdinBuffer[0] == '\004')
        {
            write(STDOUT_FILENO, byeMessage, sizeof(byeMessage));
            exit(1);
        }*/

        // use a vector to store all commands first, only then fork to create the children
        token = strtok(stdinBuffer, delimiter);
        vector[0] = token;
        int i = 1;
        while (token != NULL) {
            token = strtok(NULL, delimiter);
            vector[i] = token;
            i++;
        }
        vector[i] = NULL;

        // start the loop that will create one child per command that needs to be executed
        int j = 0;
        while (vector[j] != NULL)
        {
            // fork
            int pid = fork();
            if (pid == -1)
            {
                perror("impossible to fork");
                exit(1);
            }
            if (pid > 0) // Parent
            {
                // wait for the child that is executing the command
                if (wait(&status) >= 0)
                {
                    if (WIFEXITED(status)) // normal child termination
                    {
                        printf("Child with PID %d had a normal termination with status %d.\n", pid, WEXITSTATUS(status));
                    }
                    else if (WIFSIGNALED(status)) // abnormal child termination
                    {
                        printf("Child with PID %d had an abnormal termination with signal %d.\n", pid, WTERMSIG(status));
                    }
                }
            }
            else if (pid == 0) // Child
            {
                // call the method that will execute the command
                executeCommand(vector[j]);
            }
            j++;
        }
    }
}

void executeCommand(char *command)
{
    char *tk;
    char *argv[20];
    const char space[4] = " ";
    // use a vector to save all the arguments for the command, if there are arguments
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
    exit(0);
}

int compareStringToExit(char *str1)
{
    int c = 0;
    char *str2;
    str2 = malloc(16 * sizeof(char));
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
