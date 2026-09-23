/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event_handling.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 14:01:56 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/23 14:42:11 by alsauvan         ###   ########.fr       */
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

static void	debugging_and_refacto_phase(t_coder *coder, char *mode)
{
	t_codex	*codex;

	codex = coder->codex;
	if (strcmp(mode, "debug") == 0)
	{
		print_events(codex, coder->id, "is debugging");
		usleep(codex->time_to_debug * 1000);
	}
	if (strcmp(mode, "refacto") == 0)
	{
		print_events(codex, coder->id, "is refactoring");
		usleep(codex->time_to_refactor * 1000);
	}
}

void	*coder_events(void *arg)
{
	t_coder	*coder;
	t_codex	*codex;
	int		first;
	int		second;

	coder = (t_coder *)arg;
	codex = coder->codex;
	if (codex->nb_coders == 1)
	{
		while (!codex_stopped(codex))
			usleep(1000);
		return (NULL);
	}
	while (!codex_stopped(codex))
	{
		if (get_both_dongles(coder, &first, &second))
			break ;
		compiling_phase(coder, first, second);
		debugging_and_refacto_phase(coder, "debug");
		debugging_and_refacto_phase(coder, "refacto");
	}
	return (NULL);
}
