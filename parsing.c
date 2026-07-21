#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
int scheduler_checker(char *str)
{
    if (strcmp(str, "fifo") == 0)
        return 0;
    if (strcmp(str, "edf") == 0)
        return 0;
    return 1;
}

static long ft_digits_checker(char *str)
{
    long num;
    int i;

    num = 0;
    i = 0;
    if (!str || !str[0])
        return (-1);
    while (str[i])
    {
        if (str[i] < '0' || str[i] > '9')
            return (-1);
        num = num * 10 + (str[i] - '0');
        if (num > INT_MAX)
            return (-1);
        i++;
    }
    return (num);
}

int main(int argc, char **argv)
{
    int i;
    // int number_of_coders;
    // int time_to_burnout;
    // int time_to_compile;
    // int time_to_debug;
    // int time_to_refactor;
    // int number_of_compiles_required;
    // int dongle_cooldown;
    // char *scheduler;


    i = 1;
    if(argc != 9)
    {
        fprintf(stderr, "[ERROR][PARSING] Invalid numbers of arguments\n");
        return 1;
    }
    // number_of_coders = atoi(argv[1]);
    // time_to_burnout = atoi(argv[2]);
    // time_to_compile = atoi(argv[3]);
    // time_to_debug = atoi(argv[4]);
    // time_to_refactor = atoi(argv[5]);
    // number_of_compiles_required = atoi(argv[6]);
    // dongle_cooldown = atoi(argv[7]);
    while (i != 8)
    {
        if(ft_digits_checker (argv[i]) <= 0)
        {
            fprintf(stderr, "[ERROR][PARSING] Must be a positive digit\n");
            return 1;            
        }
        i++;
    }
    if(scheduler_checker(argv[8]) != 0)
    {
        fprintf(stderr, "[ERROR][PARSING] Scheduler must be either edf or fifo\n");
        return 1;
    }
}