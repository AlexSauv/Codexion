/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 15:01:47 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/17 17:25:38 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
# include <pthread.h>

typedef struct s_codex t_codex;
long           get_current_time(void);
void *main_coder_events(void *arg);
int init_codex(t_codex *codex);
void free_codex(t_codex *codex);
void print_log(t_codex *codex, int coder_id, char *status);


typedef struct  s_coder
{
    int         id;
    pthread_t   thread;
	long        nb_compile_done;
	long        last_compile_start;
    
    int         left_dongle;
    int         right_dongle;

	t_codex     *codex;
}   t_coder;

typedef struct  s_codex
{
    int         	nb_coders;
    long long   	time_to_burnout;
    long long   	time_to_compile;
    long long   	time_to_debug;
    long long   	time_to_refactor;
    int         	nb_comp_required;
    long long   	cooldown_dngl;
    char         	*scheduler;

	long		    start_at;
    pthread_mutex_t	log_mutex;
	int				simu_stopped;

	pthread_mutex_t *dongles;
    long            *dongle_cooldowns;

	t_coder     	*coders;
}   t_codex;

#endif