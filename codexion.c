/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 16:50:44 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/21 16:55:11 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char **argv)
{
	t_codex			codex;
	pthread_t		monitor_thread;
	long			i;

	if (!args_checker(argc, argv, &codex))
	{
		fprintf(stderr, "Format: ./codexion [nb_of_coders] [time_burnout]"
			" [time_compile] [time_debug] \n\t\t"
			" [time_refactor] [number_of_compiles_required]\n\t"
			"\t\t\t[dongle_cooldown] [scheduler]\n");
		return (1);
	}
	if (codex.nb_coders <= 0)
	{
		fprintf(stderr, "[ERROR][CODERS] must be over 0");
		return (1);
	}
	pthread_mutex_init(&codex.events_mutex, NULL);
	if (!init_codex(&codex))
	{
		pthread_mutex_destroy(&codex.events_mutex);
		return (1);
	}
	i = 0;
	while (i < codex.nb_coders)
	{
		pthread_create(&codex.coders[i].thread,
			NULL, coder_events, &codex.coders[i]);
		i++;
	}
	pthread_create(&monitor_thread, NULL, events_checker, &codex);
	i = 0;
	while (i < codex.nb_coders)
	{
		pthread_join(codex.coders[i].thread, NULL);
		i++;
	}
    pthread_join(monitor_thread, NULL);
	free_codex(&codex);
	return (0);
}
