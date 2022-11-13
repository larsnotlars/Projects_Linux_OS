#include "quiz.h"
#include <stdio.h>

int play(quiz_t *quiz)
{
    printf("Q%d: What is %s?\n", quiz->n, quiz->text);
    int num[4] = {8,4,2,1};
    char str[20];
    int ans;
    for (int i = 0; i < 4; i++)
    {
        printf("%d pt> ", num[i]);
        if (fgets(str, 20, stdin) == NULL)
        {
            finish();
        }
        sscanf(str, "%d", &ans);
        if (ans == EOF)
        {
            perror("sscanf");
            return -1;
        }
        if (ans < 0 || ans > 100)
        {
            printf("Invalid answer\n");
        }
        else
        {
            if (ans == quiz->number)
            {
                printf("Congratulations, your answer %d is correct\n",ans);
                quiz->score += num[i];
                break;
                
            }
            else if (ans > quiz->number)
            {
                printf("Too large, ");
            }
            else
            {
                printf("Too small, ");
            }
            if (i == 3)
            {
                printf("the correct answer was %d.\n" , quiz->number);
            }
            else
            {
                printf("try again.\n");
            }
            
            
        } 
    }
    printf("Your total score is %d/%d points.\n\n", quiz->score, quiz->n * 8);
    return 0;
}