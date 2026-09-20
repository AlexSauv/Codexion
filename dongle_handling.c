/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_handling.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 18:55:13 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/20 19:20:45 by alsauvan         ###   ########.fr       */
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

static int	dongle_available(t_coder *coder, int dongle)
{
	t_codex *codex;
	long	curr_time;
	int		ready;

	codex = coder->codex;
	curr_time = get_current_time() - codex->start_at;
	if (curr_time < codex->dongle_cooldowns[dongle])
		return (0);
	if (strcmp(codex->scheduler, "fifo") == 0)
		ready = first_to_request(coder, dongle);
	else
		ready = get_priority_deadline(coder, dongle);
	return (ready);
}

static void	get_a_dongle_by_fifo(t_coder *coder, int dongle)
{
	t_codex		*codex;
	long		curr_time;
	long		wait_time;

	codex = coder->codex;
	pthread_mutex_lock(&codex->events_mutex);
	codex->total_request++;
	coder->request = codex->total_request;
	pthread_mutex_unlock(&codex->events_mutex);
	while (!codex->simu_stopped)
	{
		pthread_mutex_lock(&codex->dongles[dongle]);
		curr_time = get_current_time() - codex->start_at;
		if (first_to_request(coder, dongle))
		{
			coder->request = 0;
			print_events(codex, coder->id, "has taken a dongle");
			break ;
		}
		wait_time = codex->dongle_cooldowns[dongle] - curr_time;
		pthread_mutex_unlock(&codex->dongles[dongle]);
		if (wait_time > 0)
			usleep(wait_time * 1000);
		else
			usleep(200);
	}
}

static void	get_a_dongle_by_edf(t_coder *coder, int dongle)
{
	t_codex		*codex;
	long		curr_time;
	long		wait_time;

	codex = coder->codex;
	while (!codex->simu_stopped)
	{
		pthread_mutex_lock(&codex->dongles[dongle]);
		curr_time = get_current_time() - codex->start_at;
		if (curr_time >= codex->dongle_cooldowns[dongle])
		{
			if (get_priority_deadline(coder, dongle))
			{
				print_events(codex, coder->id, "has taken a dongle");
				break ;
			}
		}
		wait_time = codex->dongle_cooldowns[dongle] - curr_time;
		pthread_mutex_unlock(&codex->dongles[dongle]);
		if (wait_time > 0)
			usleep(wait_time * 1000);
		else
			usleep(200);
	}
}

void	get_dongles(t_coder *coder, int *first_dgl, int *scnd_dgl)
{
	t_codex		*codex;

	codex = coder->codex;
    sort_dongles(coder, first_dgl, scnd_dgl);
	while (!codex->simu_stopped)
	{
		if (strcmp(codex->scheduler, "fifo") == 0)
			get_a_dongle_by_fifo(coder, *first_dgl);
		else
			get_a_dongle_by_edf(coder, *first_dgl);
		if (codex->simu_stopped)
		{
			pthread_mutex_unlock(&codex->dongles[*first_dgl]);
			return ;
		}
		if (dongle_available(coder, *scnd_dgl))
		{
			if (strcmp(codex->scheduler, "fifo") == 0)
				get_a_dongle_by_fifo(coder, *scnd_dgl);
			else
				get_a_dongle_by_edf(coder, *scnd_dgl);
			break ;
		}
		pthread_mutex_unlock(&codex->dongles[*first_dgl]);
        usleep(500);
	}
}

void	drop_dongles(t_codex *codex, int first_dgl, int scnd_dgl)
{
	codex->dongle_cooldowns[first_dgl] = (get_current_time() - codex->start_at);
	codex->dongle_cooldowns[first_dgl] += codex->cooldown_dngl;
	pthread_mutex_unlock(&codex->dongles[first_dgl]);
	codex->dongle_cooldowns[scnd_dgl] = (get_current_time() - codex->start_at);
	codex->dongle_cooldowns[scnd_dgl] += codex->cooldown_dngl;
	pthread_mutex_unlock(&codex->dongles[scnd_dgl]);
}
