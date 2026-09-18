/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_handling.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 18:55:13 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/18 17:11:46 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	get_priority_deadline(t_coder *coder, int dongle)
{
	t_codex		*codex;
	int			i;
	long		first_deadline;
	long		second_deadline;

	codex = coder->codex;
	first_deadline = coder->last_compile_start + codex->time_to_burnout;
	i = 0;
	while (i < codex->nb_coders)
	{
		if (codex->coders[i].id != coder->id)
		{
			if ((codex->coders[i].left_dongle == dongle)
				|| codex->coders[i].right_dongle == dongle)
			{
				second_deadline = (codex->coders[i].last_compile_start
						+ codex->time_to_burnout);
				if (second_deadline < first_deadline)
					return (0);
			}
		}
		i++;
	}
	return (1);
}

static int	first_to_request(t_coder *coder, int dongle)
{
	int			i;
	t_codex		*codex;

	i = 0;
	codex = coder->codex;
	while (i < codex->nb_coders)
	{
		if (codex->coders[i].id != coder->id)
		{
			if ((codex->coders[i].left_dongle == dongle
					|| codex->coders[i].right_dongle == dongle))
			{
				if (codex->coders[i].request > 0
					&& codex->coders[i].request < coder->request)
					return (0);
			}
		}
		i++;
	}
	return (1);
}

static void	get_a_dongle_by_fifo(t_coder *coder, int dongle)
{
	t_codex		*codex;
	long		curr_time;
	long		wait_time;

	codex = coder->codex;
	while (!codex->simu_stopped)
	{
		pthread_mutex_lock(&codex->dongles[dongle]);
		curr_time = get_current_time() - codex->start_at;
		if (curr_time >= codex->dongle_cooldowns[dongle]
			&& first_to_request(coder, dongle))
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

void	get_dongles(t_coder *coder, int *first_dongle, int *second_dongle)
{
	t_codex		*codex;

	codex = coder->codex;
    if (coder->left_dongle < coder->right_dongle)
		{
			*first_dongle = coder->left_dongle;
			*second_dongle = coder->right_dongle;
		}
		else
		{
			*first_dongle = coder->right_dongle;
			*second_dongle = coder->left_dongle;
		}
	if (strcmp(codex->scheduler, "fifo") == 0)
    {
        get_a_dongle_by_fifo(coder, *first_dongle);
        get_a_dongle_by_fifo(coder, *second_dongle);
    }
	else if (strcmp(codex->scheduler, "edf") == 0)
    {
		get_a_dongle_by_edf(coder, *first_dongle);
		get_a_dongle_by_edf(coder, *second_dongle);
    }
}
