#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int printenv(void)
{// function to print enviromental variables
    char ** ptr = __environ;
    for ( ptr = __environ; *ptr != NULL; ptr++)
    {
        printf("%s\n",*ptr);
        if (fflush(stdout) || ferror(stdout))
        {
            perror("printing error\n");
            exit(EXIT_FAILURE);
        }
    }
    return(EXIT_SUCCESS);
    
}

int main(int argc, char *const argv[])
{
    int arg;
    int trace = 0, print = 1;
    while ((arg = getopt(argc, argv ,"vu:")) != -1)
    {//parse through arguments of main
        switch (arg)
        {
        case 'v':
            trace++;
            break;//trance option turned on

        case 'u':
            print = 0;// dont print if not trace active
            if (trace)
            {
                fprintf(stderr,"unset : %s\n",optarg);
                if (fflush(stderr)|| ferror(stderr))
                {
                    perror("printing error\n");
                    exit(EXIT_FAILURE);
                }
                print = 1;
            }
            if (unsetenv(optarg))
            {//unset the argument of the option
                perror("error while unsetting env variable");
                exit(EXIT_FAILURE);
            }
            break;
        default:
            break;
        }
    }

    
        int i;
        for (i = optind; i < argc; i++)
        {
            if ((strstr(argv[i], "=")) != NULL)
            {//search remaining arguments for name=value pairs
                char *var, *val;
                var = strtok(argv[i], "=");
                val = strtok(NULL, "\0");
                if ((val == NULL)||(var == NULL))
                {
                    perror("ERROR:No value added after =");
                    return EXIT_FAILURE;
                }
                if (trace)
                {
                    fprintf(stderr, "setenv:    %s=%s\n", var, val);
                    if (fflush(stderr) || ferror(stderr))
                    {
                        perror("printing error\n");
                        exit(EXIT_FAILURE);
                    }
                }
                if (setenv(var, val, 0))
                {//set the found variables
                    perror("error while setting env variable");
                    exit(EXIT_FAILURE);
                }
                print= 1;
            }
            else
            {
                break;
            }
        }
        if(i < argc)
        {//looking through remaining arguments for comand
            char *cmd = malloc(sizeof(argv[i]));
            if (cmd == NULL)
            {
                perror("ERROR:cannot allocate memory");
                exit(EXIT_FAILURE);
            }
            
            strcpy(cmd,argv[i]);
            if (trace)
            {
                fprintf(stderr, "executing: %s\n", cmd);
                if (fflush(stderr) || ferror(stderr))
                {
                    perror("printing error\n");
                    exit(EXIT_FAILURE);
                }
            }
            char **cmdarg;
            cmdarg = (char**) malloc((argc - i)* sizeof(char*));
            for (int j = 0; i +j < argc ; j++)
            {// copy arguments of command to new array on heap
                cmdarg[j] = malloc(sizeof(argv[i+ j]));
                if (cmdarg[j] == NULL)
                {
                    perror("ERROR:cannot allocate memory");
                    exit(EXIT_FAILURE);
                }
                strcpy(cmdarg[j],argv[i+j]);
                if (trace)
                {
                    fprintf(stderr, "   arg[%d]= %s\n", j,cmdarg[j]);
                }
            }
            if (execvp(cmd, cmdarg))
            { // set the found variables
                perror("error while executing the command");
                exit(EXIT_FAILURE);
            }
            
            for (int j = 0; i + j < argc; j++)
            {
                free(cmdarg[j]);
            }
            free(cmd);
        }
    
    if (print)
    {
        return printenv();
    }
    return EXIT_SUCCESS;
}
