/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_state.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 17:52:38 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/22 18:16:38 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"


static int	check_burnouts(t_codex *codex, int *burned_coder)
{
	int		i;
	long	curr_time;
    long    last_comp;
	
	i = 0;
	while (i < codex->nb_coders)
	{
		curr_time = get_current_time() - codex->start_at;
        last_comp = codex->coders[i].last_compile_start;
		if ((curr_time - last_comp) > codex->time_to_burnout)
		{
			*burned_coder = codex->coders[i].id;
			return (1);
		}
		i++;
	}
	return (0);
}

static int	check_compiles_done(t_codex *codex)
{
	int	i;
	long	comp_done;
	long	comp_required;
	
	i = 0;
	if (codex->nb_comp_required == -1)
		return (0);
	while (i < codex->nb_coders)
	{
		comp_done = codex->coders[i].nb_compile_done;
		comp_required = codex->nb_comp_required;
		if (comp_done < comp_required)
			return (0);
		i++;
	}
	return (1);
}

int	codex_stopped(t_codex *codex)
{
	int	stopped;
	int burned_coder;

	pthread_mutex_lock(&codex->events_mutex);
	if (codex->simu_stopped)
	{
		pthread_mutex_unlock(&codex->events_mutex);
		return (1);
	}
	burned_coder = -1;
	if (check_burnouts(codex, &burned_coder) || check_compiles_done(codex))
	{
		codex->simu_stopped = 1;
		pthread_mutex_unlock(&codex->events_mutex);
		if (burned_coder != -1)
			print_events(codex, burned_coder, "burned out");
		return (1);
	}
	stopped = codex->simu_stopped;
	pthread_mutex_unlock(&codex->events_mutex);
	return (stopped);
}

void	*events_checker(void *arg)
{
	int				i;
	t_codex			*codex;

	codex = (t_codex *) arg;
	while (1)
	{
		i = 0;
		if (codex_stopped(codex))
        {
			while (i < codex->nb_coders)
			{
				pthread_mutex_lock(&codex->dongles[i]);
				pthread_cond_broadcast(&codex->condi[i]);
				pthread_mutex_unlock(&codex->dongles[i]);
				i++;
			}
			break;
        }
		usleep(500);
	}
	return (NULL);
}
