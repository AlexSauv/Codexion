/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 16:50:44 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/24 20:11:37 by alsauvan         ###   ########.fr       */
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

static long	create_threads(t_codex *codex)
{
	long	i;

	i = 0;
	while (i < codex->nb_coders)
	{
		if (pthread_create(&codex->coders[i].thread, NULL, coder_events,
				&codex->coders[i]) != 0)
		{
			fprintf(stderr, "[ERROR] pthread_create failed for coder %ld\n", i
				+ 1);
			return (i);
		}
		i++;
	}
	return (i);
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

static int	launch_codex(t_codex *codex)
{
	pthread_t	monitor;
	long		coders;
	int			monitor_valid;

	coders = create_threads(codex);
	monitor_valid = 0;
	if (coders == codex->nb_coders)
	{
		monitor_valid = (pthread_create(monitor, NULL, events_checker,
					&codex) == 0);
	}
	if (coders < codex->nb_coders || !monitor_valid)
	{
		pthread_mutex_lock(&codex->events_mutex);
		codex->simu_stopped = 1;
		pthread_mutex_unlock(&codex->events_mutex);
	}
	joining_threads(&codex, monitor);
	if (monitor_valid)
		pthread_join(monitor, NULL);
	return (coders == codex->nb_coders && monitor_valid);
}

int	main(int argc, char **argv)
{
	t_codex		codex;
	pthread_t	monitor_thread;

	if (!args_validator(argc, argv, &codex))
		return (1);
	pthread_mutex_init(&codex.events_mutex, NULL);
	if (!init_codex(&codex))
	{
		pthread_mutex_destroy(&codex.events_mutex);
		return (1);
	}
	if (!launch_codex(&codex))
	{
		free_codex(&codex);
		return (1);
	}
	free_codex(&codex);
	return (0);
}
