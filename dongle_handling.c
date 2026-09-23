/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_handling.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 18:55:13 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/22 18:06:18 by alsauvan         ###   ########.fr       */
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


static int check_scheduler(t_coder *coder, int dgl)
{
	t_codex *codex;

	codex = coder->codex;
	if (strcmp(codex->scheduler, "fifo") == 0)
		return (first_to_req(coder, dgl));
	return (get_priority(coder, dgl));
}

static int get_a_dongle(t_coder *coder, int dgl)
{
	t_codex *codex;
	long	curr_time;
	long	dead_time;
	int		took_it;

	codex = coder->codex;
	took_it = 0;
	while (!codex_stopped(codex))
	{
		pthread_mutex_lock(&codex->dongles[dgl]);
		curr_time = get_current_time() - codex->start_at;
		dead_time = coder->last_compile_start + codex->time_to_burnout;
		pthread_mutex_lock(&codex->events_mutex);
		update_req(codex->dongle_heaps[dgl], coder->id, curr_time, dead_time);
		pthread_mutex_unlock(&codex->events_mutex);
		if (curr_time >= codex->dongle_cooldowns[dgl]
			&& check_scheduler(coder, dgl))
			took_it = 1;
		if (took_it)
			break;
		pthread_cond_wait(&codex->condi[dgl], &codex->dongles[dgl]);
		pthread_mutex_unlock(&codex->dongles[dgl]);
	}
	remove_coder_req(codex, codex->dongle_heaps[dgl], coder->id);
	if (took_it)
		print_events(codex, coder->id, "has taken a dongle");
	return (took_it);
}

int	get_both_dongles(t_coder *coder, int *first_dgl, int *scnd_dgl)
{
	t_codex		*codex;

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
		if (dongle_available(coder, *first_dgl))
		{
			if(!get_a_dongle(coder, *first_dgl))
				return (0);
        	if (dongle_available(coder, *scnd_dgl)
				&& get_a_dongle(coder, *scnd_dgl))
				return (1);
			pthread_mutex_unlock(&codex->dongles[*first_dgl]);
			pthread_cond_broadcast(&codex->condi[*first_dgl]);
			return (0);
		}
        usleep(500);
	}
	return (0);
}

void	drop_dongles(t_codex *codex, int first_dgl, int scnd_dgl)
{
	long	curr_time;

	curr_time = get_current_time() - codex->start_at;
	codex->dongle_cooldowns[first_dgl] = curr_time + codex->cooldown_dngl;
	pthread_mutex_unlock(&codex->dongles[first_dgl]);
    pthread_cond_broadcast(&codex->condi[first_dgl]);
	curr_time = get_current_time() - codex->start_at;
	codex->dongle_cooldowns[scnd_dgl] = curr_time + codex->cooldown_dngl;
	pthread_mutex_unlock(&codex->dongles[scnd_dgl]);
    pthread_cond_broadcast(&codex->condi[scnd_dgl]);
}
