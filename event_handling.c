/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event_handling.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 14:01:56 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/21 15:15:05 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	compiling_phase(t_coder *coder, int first_dongle, int second_dongle)
{
	t_codex	*codex;

	codex = coder->codex;
	pthread_mutex_lock(&codex->events_mutex);
	coder->last_compile_start = get_current_time() - codex->start_at;
	coder->nb_compile_done++;
	pthread_mutex_unlock(&codex->events_mutex);
	print_events(codex, coder->id, "is compiling");
	usleep(codex->time_to_compile * 1000);
	drop_dongles(codex, first_dongle, second_dongle);
}

static void	debugging_and_refacto_phase(t_coder *coder, int debug, int refacto)
{
	t_codex	*codex;

	codex = coder->codex;
	if (debug == 1)
	{
		print_events(codex, coder->id, "is debugging");
		usleep(codex->time_to_debug * 1000);
	}
	if (refacto == 1)
	{
		print_events(codex, coder->id, "is refactoring");
		usleep(codex->time_to_refactor * 1000);
	}
}

void	*coder_events(void *arg)
{
	t_coder		*coder;
	t_codex		*codex;
	int			first;
	int			second;

	coder = (t_coder *) arg;
	codex = coder->codex;
	while (!codex->simu_stopped)
	{
		get_dongles(coder, &first, &second);
		compiling_phase(coder, first, second);
		debugging_and_refacto_phase(coder, 1, 0);
		debugging_and_refacto_phase(coder, 0, 1);
	}
	return (NULL);
}
