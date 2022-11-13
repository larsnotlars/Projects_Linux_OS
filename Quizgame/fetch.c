#include "quiz.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#define _POSIX_C_SOURCE 200809L

extern char* fetch(char *url)
{
    int ends[2];
    if(pipe(ends)== -1)
    {
        perror("pipes");
        return NULL;
    }
    pid_t pid = fork();
    if(pid == -1)
    {
        perror("fork");
        return NULL;
    }
    if(pid == 0) 
    {
        dup2(ends[1], STDOUT_FILENO);
        close(ends[0]);
        close(ends[1]);
        execlp("curl", "curl","-s",url, NULL);
        perror("execlp");
        return NULL;
    }
    else
    {
        close(ends[1]);
        char *msg = malloc(1000);
        if (msg == NULL)
        {
            perror("malloc");
            return NULL;
        }
        if(read(ends[0], msg, 1000) == -1)
        {
            perror("read");
            return NULL;
        }
        close(ends[0]);
        return msg;
    }
}