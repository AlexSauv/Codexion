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


// static void	get_a_dongle_by_fifo(t_coder *coder, int dgl)
// {
// 	t_codex		*codex;
// 	long		curr_time;
// 	long		dead_time;
    
// 	codex = coder->codex;
// 	curr_time = get_current_time() - codex->start_at;
// 	dead_time = curr_time - coder->last_compile_start;
// 	pthread_mutex_lock(&codex->events_mutex);
// 	update_req(&codex->dongle_heaps[dgl], coder->id, curr_time, dead_time);
// 	pthread_mutex_unlock(&codex->events_mutex);
// 	while (!codex_stopped(codex))
// 	{
//         pthread_mutex_lock(&codex->dongles[dgl]);
// 		if (first_to_request(coder, dgl))
// 		{
//             remove_coder_req(codex, &codex->dongle_heaps[dgl], coder->id);
// 			break ;
// 		}
//         pthread_cond_wait(&codex->condi[dgl], &codex->dongles[dgl]);
// 		pthread_mutex_unlock(&codex->dongles[dgl]);
// 	}
// }

// static void	get_a_dongle_by_edf(t_coder *coder, int dgl)
// {
//     t_codex		*codex;
// 	long		curr_time;
    
// 	codex = coder->codex;
// 	while (!codex_stopped(codex))
// 	{
//         pthread_mutex_lock(&codex->dongles[dgl]);
// 		curr_time = get_current_time() - codex->start_at;
// 		if (curr_time >= codex->dongle_cooldowns[dgl])
// 		{
//             if (get_priority_deadline(coder, dgl))
// 				break ;
// 		}
//         pthread_cond_wait(&codex->condi[dgl], &codex->dongles[dgl]);
// 		pthread_mutex_unlock(&codex->dongles[dgl]);
// 	}
// }

static void get_a_dongle(t_coder *coder, int dgl)
{
	t_codex *codex;
	long	curr_time;
	long	dead_time;

	codex = coder->codex;
	while (!codex_stopped(codex))
	{
		pthread_mutex_lock(&codex->dongles[dgl]);
		curr_time = get_current_time() - codex->start_at;
		dead_time = coder->last_compile_start + codex->time_to_burnout;
		pthread_mutex_lock(&codex->events_mutex);
		update_req(codex->dongle_heaps[dgl], coder->id, curr_time, dead_time);
		pthread_mutex_unlock(&codex->events_mutex);
		if (curr_time >= codex->dongle_cooldowns[dgl])
		{
			if (strcmp(codex->scheduler, "fifo") == 0 && first_to_req(coder, dgl))
				break;
			if (strcmp(codex->scheduler, "edf") == 0 && get_priority(coder, dgl))
				break;
		}
		pthread_cond_wait(&codex->condi[dgl], &codex->dongles[dgl]);
		pthread_mutex_unlock(&codex->dongles[dgl]);
	}
	remove_coder_req(codex, codex->dongle_heaps[dgl], coder->id);
	print_events(codex, coder->id, "has taken a dongle");
}

void	get_both_dongles(t_coder *coder, int *first_dgl, int *scnd_dgl)
{
	t_codex		*codex;

	codex = coder->codex;
    sort_dongles(coder, first_dgl, scnd_dgl);
	while (!codex_stopped(codex))
	{
		if (dongle_available(coder, *first_dgl))
		{
			get_a_dongle(coder, *first_dgl);
        	if (dongle_available(coder, *scnd_dgl))
        	{
				get_a_dongle(coder, *scnd_dgl);
				break ;
			}
			else
			{
				pthread_mutex_unlock(&codex->dongles[*first_dgl]);
				pthread_cond_broadcast(&codex->condi[*first_dgl]);
			}
		}
        usleep(500);
	}
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
