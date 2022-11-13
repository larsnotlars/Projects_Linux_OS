#include "quiz.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/signal.h>

#define _POSIX_C_SOURCE 200809L


int score = 0;
int number = 1;

void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        finish();
    }
}

void finish()
{
    printf("\n\nThanks for playing today.\nYour final score is %d/%d points.\n", score, number  * 8);
    if (ferror(stdout) || ferror(stdin) || fflush(stdin) == EOF|| fflush(stdout) == EOF)
    {
        perror("stdout");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}



int main(int argc, char const *argv[])
{
    if (signal(SIGINT, sig_handler) == SIG_ERR )
    {
        printf("signal error");
        exit(EXIT_FAILURE);
    }
    printf("Answer questions with numbers in the range [1..100].\nYou score points for each correctly answered question.\nIf you need multiple attempts to answer a question, the points you score for a correct answer go down.\n\n");
    char* test;
    quiz_t quiz;
    quiz.score = 0;
    quiz.n = 1;

    while(1)
    {
        test = fetch("http://numbersapi.com/random/math?min=1&max=100&fragment&json");
        if (test == NULL)
        {
            perror("fetch");
            exit(EXIT_FAILURE);
        }
        if (parse(test, &quiz) == -1)
        {
            perror("parse");
            exit(EXIT_FAILURE);
        }
        play(&quiz);
        quiz.n++;
        number++;
        score = quiz.score;
    }
    
    return 0;
}
