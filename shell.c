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

    while (1)
    {
        stdinBuffer = malloc(1024 * sizeof(char));
        token = malloc(64 * sizeof(char));
        // reading command line from the user
        read(STDIN_FILENO, stdinBuffer, 1024);
        // if the input is "exit" or CTR+D, print "Bye bye" and terminate the shell
        if (!compareStringToExit(stdinBuffer))
        {
            write(STDOUT_FILENO, byeMessage, sizeof(byeMessage));
            exit(1);
        }
        /* TODO The CTRL+D part isn't working
        if (stdinBuffer[0] == '\004')
        {
            write(STDOUT_FILENO, byeMessage, sizeof(byeMessage));
            exit(1);
        }*/

        // IDEA: maybe put all commands in a VECTOR first, and then the parent deals with this vector

        // get the first command
        token = strtok(stdinBuffer, delimiter);
        free(stdinBuffer);
        // get the other commands, if there is at least one more
        while (token != NULL) {
            // Fork
            int pid = fork();
            if (pid == -1)
            {
                perror("impossible to fork");
                exit(1);
            }
            if (pid > 0) // Parent
            {
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
                    // the "pid" variable here is the pid of the child returned to the parent
                    // the "write" function here should get an array of integers
                    //printf("Am I getting here? Status: %d, PID: %d\n", WEXITSTATUS(status), pid);
                    // int s = WEXITSTATUS(status);
                    // write(STDOUT_FILENO, &s, sizeof(s));
                    // write(STDOUT_FILENO, &pid, sizeof(pid));
                }
                token = strtok(NULL, delimiter);
            }
            else if (pid == 0) // Child
            {
                executeCommand(token);
            }
        }
    }
}

void executeCommand(char *command)
{
    char *tk;
    char *argv[20];
    const char space[4] = " ";
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
