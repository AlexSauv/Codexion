/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 15:01:47 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/23 13:50:09 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H
# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

typedef struct s_codex	t_codex;
typedef struct s_coder	t_coder;
typedef struct s_heap	t_heap;
typedef struct s_req	t_req;

int						args_checker(int argc, char **argv, t_codex *codex);
t_heap					*heap_create(int capacity);
void					heap_freed(t_heap *heap);
void					heap_swap(t_heap *heap, int req_one, int req_two);
void					update_req(t_codex *codex, t_heap *heap, t_req new_req);
void					remove_req(t_codex *codex, t_heap *heap, int coder_id);
int						get_coder_id(t_heap *heap);
int						init_codex(t_codex *codex);
long					get_current_time(void);
void					*coder_events(void *arg);
void					*events_checker(void *arg);
int						codex_stopped(t_codex *codex);
void					free_codex(t_codex *codex);
void					print_events(t_codex *codex, int coder_id,
							char *status);
int						get_priority(t_codex *codex, t_req *first, t_req *scnd);
int						dongle_available(t_coder *coder, int dongle);
int						get_both_dongles(t_coder *coder, int *first_dgl,
							int *scnd_dgl);
void					drop_dongles(t_codex *codex, int first_dgl,
							int scnd_dgl);

typedef struct s_req
{
	int					coder_id;
	long				req_time;
	long				deadline;
}						t_req;

typedef struct s_heap
{
	t_req				*req;
	int					*coder_ids;
	int					capacity;
	int					size;
}						t_heap;

typedef struct s_coder
{
	int					id;
	pthread_t			thread;
	long				nb_compile_done;
	long				last_compile_start;

	int					left_dongle;
	int					right_dongle;

	t_codex				*codex;
}						t_coder;

typedef struct s_codex
{
	int					nb_coders;
	long long			time_to_burnout;
	long long			time_to_compile;
	long long			time_to_debug;
	long long			time_to_refactor;
	int					nb_comp_required;
	long long			cldwn_dgl;
	char				*scheduler;

	long				start_at;
	pthread_mutex_t		events_mutex;
	int					mutexes;
	int					simu_stopped;

	pthread_mutex_t		*dongles;
	pthread_cond_t		*condi;
	long				*dgl_cldwns;
	t_heap				**dongle_heaps;
	long				total_request;

	t_coder				*coders;
}						t_codex;

#endif