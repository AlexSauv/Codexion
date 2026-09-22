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


static int	check_burnouts(t_codex *codex)
{
	int		i;
	long	curr_time;
    long    last_comp;
	
	i = 0;
	while (i < codex->nb_coders)
	{
        pthread_mutex_lock(&codex->events_mutex);
		curr_time = get_current_time() - codex->start_at;
        last_comp = codex->coders[i].last_compile_start;
        pthread_mutex_unlock(&codex->events_mutex);
		if ((curr_time - last_comp) > codex->time_to_burnout)
		{
			print_events(codex, codex->coders[i].id, "burned out");
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
		pthread_mutex_lock(&codex->events_mutex);
		comp_done = codex->coders[i].nb_compile_done;
		comp_required = codex->nb_comp_required;
		pthread_mutex_unlock(&codex->events_mutex);
		if (comp_done < comp_required)
			return (0);
		i++;
	}
	return (1);
}

int	codex_stopped(t_codex *codex)
{
	if (check_burnouts(codex) || check_compiles_done(codex))
	{
		pthread_mutex_lock(&codex->events_mutex);
		codex->simu_stopped = 1;
		pthread_mutex_unlock(&codex->events_mutex);
		return (1);
	}
	return (0);
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
				pthread_cond_broadcast(&codex->condi[i]);
				i++;
			}
			return (NULL);
        }
		usleep(500);
	}
	return (NULL);
}
