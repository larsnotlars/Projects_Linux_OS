#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

// compile with "gcc -Wall -o prisoner prisoner.c -lpthread"
// struct holds information to pass to thread
struct prisoner
{
    int num;
    int s;
    int ret;
};

//holds information for every drawer
struct drawer
{
    int num;
    pthread_mutex_t mutex;
};

//global variables
static struct drawer drawers[100];
static struct prisoner prisoners[100];
static pthread_mutex_t global = PTHREAD_MUTEX_INITIALIZER;

static int seed;//needs to be incremented whenever new sequence is made

static int glob_dumb_counter = 0;
static int loc_dumb_counter = 0;
static int glob_str_counter = 0;
static int loc_str_counter = 0;

static void mutex_lock(pthread_mutex_t *mutex)
{
    int rc = pthread_mutex_lock(mutex);
    if (rc)
    {
        fprintf(stderr, "pthread_mutex_lock(): %s\n", strerror(rc));
        exit(EXIT_FAILURE);
    }
}

static void mutex_unlock(pthread_mutex_t *mutex)
{
    int rc = pthread_mutex_unlock(mutex);
    if (rc)
    {
        fprintf(stderr, "pthread_mutex_unlock(): %s\n", strerror(rc));
        exit(EXIT_FAILURE);
    }
}

//function to take array arr of length n and create a random sequence in it using Fischer Yates algorithm
void randarr(int * arr, int n)
{
    int r, temp;
    for (int i = 0; i < n; i++)
    {
        arr[i] = i;
    }
    
    for (int i = 0; i < n; i++)
    {
        r = rand() % (n-i);
        temp = arr[i];
        arr[i] = arr[i+r];
        arr[i+r] = temp;
    }
}

//========================================================================
//functions for the threads:

void * glob_dum_pris(void *ptr)
{
    struct prisoner *args = (struct prisoner *) ptr;
    srand(args->s);
    int num = args->num;
    int seq[100];
    args->ret = 0;
    randarr(seq, 100);
    for (int i = 0; i < 50; i++)
    {
        mutex_lock(&global);
        if (drawers[seq[i]].num == num )
        {
            args->ret = 1;
        }
        mutex_unlock(&global);
    }
    return NULL;
}

void *glob_str_pris(void *ptr)
{
    struct prisoner *args = (struct prisoner *)ptr;
    srand(args->s);
    int num = args->num;
    int temp = args->num;
    args->ret = 0;
    
    for (int i = 0; i < 50; i++)
    {
        mutex_lock(&global);
        if (drawers[temp].num == num)
        {
            args->ret = 1;
        }
        temp = drawers[temp].num;
        mutex_unlock(&global);
    }
    return NULL;
}

void * loc_dum_pris(void *ptr)
{
    struct prisoner *args = (struct prisoner *)ptr;
    srand(args->s);
    int num = args->num;
    int seq[100];
    args->ret = 0;
    randarr(seq, 100);
    for (int i = 0; i < 50; i++)
    {
        mutex_lock(&drawers[seq[i]].mutex);
        if (drawers[seq[i]].num == num)
        {
            args->ret = 1;
        }
        mutex_unlock(&drawers[seq[i]].mutex);
    }
    return NULL;
}

void* loc_str_pris(void *ptr)
{
    struct prisoner *args = (struct prisoner *)ptr;
    int num = args->num;
    int temp1 = args->num;
    int temp2;
    args->ret = 0;

    for (int i = 0; i < 50; i++)
    {
        mutex_lock(&drawers[temp1].mutex);
        if (drawers[temp1].num == num)
        {
            args->ret = 1;
        }
        
        temp2 = temp1;
        temp1 = drawers[temp1].num;
        mutex_unlock(&drawers[temp2].mutex);
    }
    return NULL;
}

//==============================================================================

//run threads to create the prisioners 
void run_threads(int n , void* (*work) (void*), int* counter)
{
    pthread_t tids[100];
    int outcome = 1;
    for (int i = 0; i < n; i++)
    {
        prisoners[i].s = seed;
        pthread_create(&tids[i],NULL,work,&prisoners[i]);
        seed++;
    }
    for (int i = 0; i < n; i++)
    {
        pthread_join(tids[i],NULL);
        if (prisoners[i].ret == 0)
        {//check if one loses because all loose then
            outcome = 0;
        }
        else
        {
            prisoners[i].ret = 0; // reset all prisoners
        }
    }
    *counter += outcome; // add successful executions to global counter
}

//function to run the game with n proc-threads and time it
static double timeit(int n, void *(*proc)(void *), int *counter)
{
    clock_t t1, t2;
    t1 = clock();
    run_threads(n, proc, counter);
    t2 = clock();
    return ((double)t2 - (double)t1) / CLOCKS_PER_SEC * 1000;
}

//function to create the random drawers and record the times of the games
void test(int n, void* (*func) (void*), int* counter)
{
    int temparr[100];
    double time = 0;
    for (int i = 0; i < n; i++)
    {
        srand(seed);
        randarr(temparr, 100);
        seed++;
        for (int i = 0; i < 100; i++)
        {
            drawers[i].num = temparr[i];
        }
        time += timeit(100, func, counter);
    }
    printf("%d/%d   wins= %.1lf%% and time %.3lf\n", *counter, n, *counter*100 / (double)n, time);
}

//functions to call all games test runs one after another and print their counters
int run(int n)
{
    printf("method random_global            ");
    test(n,glob_dum_pris,&glob_dumb_counter);
    printf("method random_local             ");
    test(n,loc_dum_pris, &loc_dumb_counter);
    printf("method startegy_global          ");
    test(n,glob_str_pris,&glob_str_counter);
    printf("method strategy_local           ");
    test(n,loc_str_pris,&loc_str_counter);

    if (fflush(stdin) || ferror(stdin))
    {
        perror("Error printing onto stderr");
        exit(EXIT_FAILURE);
    }

    return 0;
}

int main(int argc, char  *argv[])
{
    //initialize drawers
    for (int i = 0; i < 100; i++)
    {
        drawers[i].mutex =(pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    }
    for (int i = 0; i < 100; i++)
    {
        prisoners[i].num = i;
        prisoners[i].ret = 0;
    }
    
    seed = time(NULL);




    int opt, n = 1;
    while ((opt = getopt(argc, argv, "n:s:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            n = atoi(optarg);
            break;
        case 's':
            seed = atoi(optarg);
            break;
        default:
            break;
        }
    }

    return run(n);
}
