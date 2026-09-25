/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_handling.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 18:55:13 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/25 11:32:45 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	sort_dongles(t_coder *coder, int *first_dgl, int *scnd_dgl)
{
	if (coder->left_dongle < coder->right_dongle)
	{
		*first_dgl = coder->left_dongle;
		*scnd_dgl = coder->right_dongle;
	}
	else
	{
		*first_dgl = coder->right_dongle;
		*scnd_dgl = coder->left_dongle;
	}
}

static int	try_a_dongle(t_coder *coder, t_codex *codex, int dgl, long req_time)
{
	long			now;
	long			dead;
	int				took_it;

	now = get_current_time() - codex->start_at;
	dead = coder->last_compile_start + codex->time_to_burnout;
	pthread_mutex_lock(&codex->events_mutex);
	update_req(codex, codex->dgl_heaps[dgl],
		(t_req){coder->id, req_time, dead});
	took_it = 0;
	if (now >= codex->dgl_cldwns[dgl] && check_coder_id(coder, dgl))
		took_it = 1;
	pthread_mutex_unlock(&codex->events_mutex);
	return (took_it);
}

static int	get_a_dongle(t_coder *coder, int dgl)
{
	t_codex			*codex;
	long			req_time;
	int				took_it;
	struct timespec	ts;

	codex = coder->codex;
	took_it = 0;
	req_time = get_current_time() - codex->start_at;
	while (!codex_stopped(codex))
	{
		pthread_mutex_lock(&codex->dongles[dgl]);
		took_it = try_a_dongle(coder, codex, dgl, req_time);
		if (took_it)
			break ;
		get_timeout(&ts, 5);
		pthread_cond_timedwait(&codex->condi[dgl], &codex->dongles[dgl], &ts);
		pthread_mutex_unlock(&codex->dongles[dgl]);
	}
	remove_req(codex, codex->dgl_heaps[dgl], coder->id);
	if (took_it)
		print_events(codex, coder->id, "has taken a dongle");
	return (took_it);
}

int	get_both_dongles(t_coder *coder, int *first_dgl, int *scnd_dgl)
{
	t_codex	*codex;

	codex = coder->codex;
	if (codex->nb_coders == 1)
	{
		while (!codex_stopped(codex))
			usleep(500);
		return (0);
	}
	sort_dongles(coder, first_dgl, scnd_dgl);
	while (!codex_stopped(codex))
	{
		if (dgl_is_free(coder, *first_dgl))
		{
			if (!get_a_dongle(coder, *first_dgl))
				return (0);
			if (dgl_is_free(coder, *scnd_dgl) && get_a_dongle(coder, *scnd_dgl))
				return (1);
			pthread_mutex_unlock(&codex->dongles[*first_dgl]);
			pthread_cond_broadcast(&codex->condi[*first_dgl]);
			continue ;
		}
		usleep(500);
	}
	return (0);
}

void	drop_dongles(t_codex *codex, int first_dgl, int scnd_dgl)
{
	long	curr_time;

	curr_time = get_current_time() - codex->start_at;
	pthread_mutex_lock(&codex->events_mutex);
	codex->dgl_cldwns[first_dgl] = curr_time + codex->cldwn_dgl;
	pthread_mutex_unlock(&codex->events_mutex);
	pthread_mutex_unlock(&codex->dongles[first_dgl]);
	pthread_cond_broadcast(&codex->condi[first_dgl]);
	curr_time = get_current_time() - codex->start_at;
	pthread_mutex_lock(&codex->events_mutex);
	codex->dgl_cldwns[scnd_dgl] = curr_time + codex->cldwn_dgl;
	pthread_mutex_unlock(&codex->events_mutex);
	pthread_mutex_unlock(&codex->dongles[scnd_dgl]);
	pthread_cond_broadcast(&codex->condi[scnd_dgl]);
}
