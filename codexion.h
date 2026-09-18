/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 15:01:47 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/18 17:59:50 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>
# include <limits.h>
# include <pthread.h>

typedef struct s_codex	t_codex;
typedef struct s_coder	t_coder;
int				args_checker(int argc, char **argv, t_codex *codex);
int				init_codex(t_codex *codex);
long			get_current_time(void);
void			*coder_events(void *arg);
void			*events_checker(void *arg);
void			free_codex(t_codex *codex);
void			print_events(t_codex *codex, int coder_id, char *status);
void			get_dongles(t_coder *coder, int *first_dongle, int *second_dongle);
void			drop_a_dongle(t_codex *codex, int dongle);

typedef struct s_scheduler
{
	int				coder_id;
	long			arrival_time;
	long			deadline;
	int				waiting;
}	t_scheduler;

typedef struct s_coder
{
	int			id;
	pthread_t	thread;
	long		nb_compile_done;
	long		last_compile_start;
	long		request;

	int			left_dongle;
	int			right_dongle;

	t_codex		*codex;
}	t_coder;

typedef struct s_codex
{
	int					nb_coders;
	long long			time_to_burnout;
	long long			time_to_compile;
	long long			time_to_debug;
	long long			time_to_refactor;
	int					nb_comp_required;
	long long			cooldown_dngl;
	char				*scheduler;

	long				start_at;
	pthread_mutex_t		events_mutex;
    int                 mutexes;
	int					simu_stopped;

	pthread_mutex_t		*dongles;
	long				*dongle_cooldowns;

	t_coder				*coders;
}	t_codex;

#endif