/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checker_state.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 17:52:38 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/18 14:30:32 by alsauvan         ###   ########.fr       */
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
		curr_time = get_current_time() - codex->start_at;
		if ((curr_time - codex->coders[i].last_compile_start)
			> codex->time_to_burnout)
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
	int	done;

	i = 0;
	done = 1;
	if (codex->nb_comp_required == -1)
		return (0);
	while (i < codex->nb_coders && i < codex->nb_comp_required == 0)
	{
		if (codex->coders[i].nb_compile_done < codex->nb_comp_required)
		{
			done = 0;
			break ;
		}
		i++;
	}
	return (done);
}

void	*events_checker(void *arg)
{
	t_codex			*codex;

	codex = (t_codex *) arg;
	while (1)
	{
		if (codex->simu_stopped)
			break ;
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
