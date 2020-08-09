#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/time.h>

/* A function that checks if the user has entered "exit" as the input */
int compareStringToExit(char *str1);

int main(int argc, char *argv[])
{
    int status;
    char *stdinBuffer;
    const char byeMessage[] = "Bye bye.\n";

    while (1)
    {
        // Fork
        int pid = fork();
        if (pid == -1)
        {
            perror("impossible to fork");
            exit(1);
        }
        if (pid > 0) // Parent
        {
            printf("I'm the parent before calling wait(). My PID is %d.\n", getpid());
            stdinBuffer = malloc(1024 * sizeof(char));
            // reading command line from the user
            read(STDIN_FILENO, stdinBuffer, 1024);
            // if the input is "exit" or CTR+D, print "Bye bye" and terminate the shell
            if (!compareStringToExit(stdinBuffer))
            {
                write(STDOUT_FILENO, byeMessage, sizeof(byeMessage));
                exit(1);
            }
            // the parent calls wait() to wait for its child
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
            printf("I'm the child printing something. My PID is %d and my parent's PID is %d.\n", getpid(), getppid());
            printf("Content of stdinBuffer: %s\n", stdinBuffer);
            exit(23);
        }
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
