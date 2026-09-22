/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 15:01:47 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/22 16:26:07 by alsauvan         ###   ########.fr       */
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
typedef struct s_heap	t_heap;
int				args_checker(int argc, char **argv, t_codex *codex);
t_heap			*heap_create(int capacity);
void 			update_req(t_heap *heap, int coder_id, long req, long deadline);
void			remove_coder_req(t_codex *codex, t_heap *heap, int coder_id);
int				init_codex(t_codex *codex);
long			get_current_time(void);
void			*coder_events(void *arg);
void			*events_checker(void *arg);
int				codex_stopped(t_codex *codex);
void			free_codex(t_codex *codex);
void			print_events(t_codex *codex, int coder_id, char *status);
int				get_priority_deadline(t_coder *coder, int dongle);
int				first_to_request(t_coder *coder, int dongle);
int				dongle_available(t_coder *coder, int dongle);
void			get_both_dongles(t_coder *coder, int *first_dgl, int *scnd_dgl);
void			drop_dongles(t_codex *codex, int first_dgl, int scnd_dgl);

typedef struct s_req
{
	int				coder_id;
	long			req_time;
	long			deadline;
}	t_req;

typedef struct s_heap
{
	t_req	*req;
	int		capacity;
	int		size;
}	t_heap;

typedef struct s_coder
{
	int			id;
	pthread_t	thread;
	long		nb_compile_done;
	long		last_compile_start;

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
	int					mutexes;
	int					simu_stopped;
	
	pthread_mutex_t		*dongles;
	pthread_cond_t		*condi;
	long				*dongle_cooldowns;
    t_heap				*dongle_heaps;
	long				total_request;

	t_coder				*coders;
}	t_codex;

#endif