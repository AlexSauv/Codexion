/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 16:50:44 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/24 16:52:07 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	args_validator(int argc, char **argv, t_codex *codex)
{
	if (!args_checker(argc, argv, codex))
	{
		fprintf(stderr,
			"Format: ./codexion [nb_of_coders] [time_burnout]"
			" [time_compile] [time_debug] \n\t\t"
			" [time_refactor] [number_of_compiles_required]\n\t"
			"\t\t\t[dongle_cooldown] [scheduler]\n");
		return (0);
	}
	if (codex->nb_coders <= 0)
	{
		fprintf(stderr, "[ERROR][CODERS] must be over 0");
		return (0);
	}
	return (1);
}

static void	create_threads(t_codex *codex, pthread_t *monitor_thread)
{
	long	i;

	i = 0;
	while (i < codex->nb_coders)
	{
		pthread_create(&codex->coders[i].thread, NULL, coder_events,
			&codex->coders[i]);
		i++;
	}
	pthread_create(monitor_thread, NULL, events_checker, &codex);
}

static void	joining_threads(t_codex *codex, pthread_t monitor_thread)
{
	long	i;

	i = 0;
	while (i < codex->nb_coders)
	{
		pthread_join(codex->coders[i].thread, NULL);
		i++;
	}
	pthread_join(monitor_thread, NULL);
}

int	main(int argc, char **argv)
{
	t_codex		codex;
	pthread_t	monitor_thread;

	if (!args_validator(argc, argv, &codex))
		return (1);
	codex.dongles = NULL;
	codex.dgl_cldwns = NULL;
	codex.coders = NULL;
	codex.condi = NULL;
	codex.dgl_heaps = NULL;
	codex.mutexes = 0;
	pthread_mutex_init(&codex.events_mutex, NULL);
	if (!init_codex(&codex))
	{
		pthread_mutex_destroy(&codex.events_mutex);
		return (1);
	}
	create_threads(&codex, &monitor_thread);
	joining_threads(&codex, monitor_thread);
	free_codex(&codex);
	return (0);
}
