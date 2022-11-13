#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

struct subint
{
    int begin;
    int end;
};

static int is_perfect(uint64_t num)
{
    uint64_t i, sum;
    if (num < 2)
    {
        return 0;
    }
    for (i = 2, sum = 1; i * i <= num; i++)
    {
        if (num % i == 0)
        {
            sum += (i * i == num) ? i : i + num / i;
        }
    }
    return (sum == num);
}

static void* thread_interval(void * data)
{
    //read the interval and run perfet on every number in it
    struct subint * a = (struct subint *) data ;
    for (int i = a->begin; i < a->end; i++)
    {
        if(is_perfect(i))
        {
            printf("%d\n", i);
            if(fflush(stdin)||ferror(stdin))
            {
                perror("error printing numbers");
            }
        }
    }
    return NULL;
}

void create_threads(int num, int min, int max,int vflag)
{
    //create the structures to pass to the threads
    int interval = (max - min) / num;
    pthread_t *tids = (pthread_t *)malloc(sizeof(pthread_t)*num);
    if (tids == NULL)
    {
        perror("Cannot allocate memory");
        exit(EXIT_FAILURE);
    }

    struct subint *array = (struct subint *)malloc(num * sizeof(struct subint));
    if (array == NULL)
    {
        perror("Cannot allocate memory");
        exit( EXIT_FAILURE);
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // create threads with equal long intervals passed to them
    int i;
    for (i = 0; i < num - 1; i++)
    {
        array[i].begin = min + i * interval;
        array[i].end = min + (i + 1) * interval - 1;
        if(vflag)
        {
            fprintf(stderr,"perfect: t%d searching: [%d,%d]\n",i,array[i].begin, array[i].end);
        }

        if((pthread_create(&tids[i], NULL, thread_interval, &array[i])) != 0)
        {
            perror("Error creating a thread");
            exit(EXIT_FAILURE);
        }
    }

    array[i].begin = min + i * interval;
    array[i].end = max;
    if (vflag)
    {
        fprintf(stderr, "perfect: t%d searching: [%d,%d]\n", i, array[i].begin, array[i].end);
    }
    if ((pthread_create(&tids[i], NULL, thread_interval, &array[i])) != 0)
    {
        perror("Error creating a thread");
        exit(EXIT_FAILURE);
    }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//wait for threads 
    for (i = 0; i < num; i++)
    {
        if((pthread_join(tids[i], NULL)) != 0)
        {
            perror("error joining threads");
            exit(EXIT_FAILURE);
        }

        if (vflag)
        {
            fprintf(stderr, "perfect: t%d finishing\n", i);
        }
    }

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//free memory
    free(tids);
    free(array);
}


int main(int argc, char  *argv[])
{
    int opt, min = 1, max = 10000, vflag = 0, num = 1;
    while ((opt = getopt(argc, argv , "s:e:t:v")) != -1) 
    {
        switch (opt)
        {
        case 's':
            min = atoi(optarg);
            break;
        case 'e':
            max = atoi(optarg);
            break;
        case 't':
            num = atoi(optarg);
            break;
        case 'v':
            vflag = 1;
            break;
        default:
            break;
        }
    }
    
    create_threads(num,min,max,vflag);

    return EXIT_SUCCESS;
}
