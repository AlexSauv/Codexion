#ifndef CODEXION_H
# define CODEXION_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
# include <pthread.h>

typedef struct  s_heap
{
    t_coder **arr;
    int stock;
    int size;
} t_heap;

typedef struct s_dongle
{
    int id;
    int is_not_available;
    long long be_available_at;
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    t_heap          *waiting;

}   t_dongle;


typedef struct s_coder
{
    int         id;
    int         nb_compiles;
    long long   req_time;
    long long   last_compile;
    pthread_t   thread;
    
    t_dongle    *left_dongle;
    t_dongle    *right_dongle;
    
    t_codex     *codex;
} t_coder;

typedef struct s_codex
{
    int         nb_coders;
    int         scheduler_type;
    int         nb_comp_required;
    long long   cooldown_dngl;
    long long   time_to_compile;
    long long   time_to_debug;
    long long   time_to_refactor;
    long long   time_to_burnout;

    t_coder     *coders;
    t_dongle    *dongles;
} t_codex;

#endif