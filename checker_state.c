/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_state.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 17:52:38 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/18 18:09:34 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	check_burnouts(t_codex *codex)
{
	int		i;
	long	curr_time;

	i = 0;
	while (i < codex->nb_coders)
	{
        pthread_mutex_lock(&codex->events_mutex);
		curr_time = get_current_time() - codex->start_at;
		if ((curr_time - codex->coders[i].last_compile_start) > codex->time_to_burnout)
		{
            print_events(codex, codex->coders[i].id, "burned out");
            pthread_mutex_unlock(&codex->events_mutex);
			return (1);
		}
        pthread_mutex_unlock(&codex->events_mutex);
		i++;
	}
	return (0);
}

static int	check_compiles_done(t_codex *codex)
{
	int	i;

	i = 0;
	if (codex->nb_comp_required == -1)
		return (0);
	while (i < codex->nb_coders)
	{
        pthread_mutex_lock(&codex->events_mutex);
		if (codex->coders[i].nb_compile_done < codex->nb_comp_required)
		{
			pthread_mutex_unlock(&codex->events_mutex);
			return (0);
		}
        pthread_mutex_unlock(&codex->events_mutex);
		i++;
	}
	return (1);
}

void	*events_checker(void *arg)
{
	t_codex			*codex;

	codex = (t_codex *) arg;
	while (1)
	{
        pthread_mutex_lock(&codex->events_mutex);
		if (codex->simu_stopped)
        {
            pthread_mutex_unlock(&codex->events_mutex);
			break ;
        }
        pthread_mutex_unlock(&codex->events_mutex);
		if (check_burnouts(codex))
			return (NULL);
		if (check_compiles_done(codex))
		{
            pthread_mutex_lock(&codex->events_mutex);
			codex->simu_stopped = 1;
			pthread_mutex_unlock(&codex->events_mutex);
			return (NULL);
		}
		usleep(500);
	}
	return (NULL);
}
