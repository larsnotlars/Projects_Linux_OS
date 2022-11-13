#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>

#define MAXARG 64


int helper(char * cmd, char ** args,int tflag)
{//helper funciton to execute command
    int status = 0;

    if (tflag)
    {
        for (int i = 0; args[i] != NULL; i++)
        {
            fprintf(stderr,"%s ", args[i]);
        }
        printf("\n");
    }
    //clone process
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("ERROR executing the fork function");
    }
    
    
    if (pid == 0)
    {//execute command in child process
        execvp(cmd,args);
        perror("Error executing the command");
    }
    else
    {//wait for child
        if((waitpid(pid,&status, 0)) == -1)
        {
            fprintf(stderr,"error waiting for the child:%s",strerror(errno));
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    int arg, n = MAXARG, tflag =0;
    //parse input for commands and options
    while ((arg = getopt(argc,argv, "n:t")) != -1)
    {
        switch (arg)
        {
        case 'n':
            n = atoi(optarg);
            break;
        case 't':
            tflag = 1;
            break;

        default:
            break;
        }
    }

    //set standard arguments vector for execvp
    char echo[5] = {"/bin/echo"};

    char *std_argv[MAXARG] = {strndup(echo, sizeof(echo))};
    if (std_argv == NULL)
    {
        perror("ERROR allocating memory");
    }
    

    //build command array in case it is not standard
    int i = 0;
    if (optind == argc)
    {
        i++;
    }
    else
    {
        while (i + optind != argc)
        {
            if((std_argv[i] = strndup(argv[i + optind], sizeof(argv[i + optind])))== NULL)
            {
                perror("ERROR allocating memory");
            }
            i++;
        }
    }
    std_argv[i+1] = NULL;

    int min = i, max;

    if (n + min  + 1    < MAXARG)
    {
        max = n +min +1 ;
    }
    else
    {
        max = MAXARG;
    }

    ssize_t val;
    size_t len = 0;
    i = min;
    char *tmp;

    //parse arguments until either MAXARG is reached or the max number set by user
    while ((val = getline(&tmp, &len, stdin)) != -1)
    {
        if (tmp[val -1] == '\n')
        {
            val--;
        }

        if((std_argv[i] = strndup(tmp,val))==NULL)
        {
            perror("ERROR allocating memory");
        }
        if (i == max-2)
        { // intermidiate execution
            std_argv[++i] = NULL;
            helper(std_argv[0], std_argv, tflag);
            i = min;
        }
        else
        {
            i++;
        }
    }
    //final execution to pick up left over arguments
    if (i != min)
    {
        std_argv[i] = NULL;
        helper(std_argv[0], std_argv, tflag);
    }

    for (int i = 0; i < MAXARG; i++)
    {
        if (std_argv[i] != NULL)
        {
            free(std_argv[i]);
        }
    }
    if (fflush(stderr)||ferror(stderr))
    {
        perror("Error printing onto stderr");
    }
    return 0;
}
