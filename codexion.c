/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 16:50:44 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/18 18:11:23 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	drop_a_dongle(t_codex *codex, int dongle)
{
	codex->dongle_cooldowns[dongle] = (get_current_time() - codex->start_at);
	codex->dongle_cooldowns[dongle] += codex->cooldown_dngl;
	pthread_mutex_unlock(&codex->dongles[dongle]);
}

void	*coder_events(void *arg)
{
	t_coder		*coder;
	t_codex		*codex;
    int         first;
    int         second;

	coder = (t_coder *) arg;
	if (coder->codex)
		codex = coder->codex;
	else
		return (NULL);
	coder->last_compile_start = get_current_time() - codex->start_at;
	while (!codex->simu_stopped)
	{
		get_dongles(coder, &first, &second);
        pthread_mutex_lock(&codex->events_mutex);
		coder->last_compile_start = get_current_time() - codex->start_at;
        pthread_mutex_unlock(&codex->events_mutex);
		print_events(codex, coder->id, "is compiling");
		usleep(codex->time_to_compile * 1000);
        pthread_mutex_lock(&codex->events_mutex);
		coder->nb_compile_done++;
        pthread_mutex_unlock(&codex->events_mutex);
		drop_a_dongle(codex, first);
		drop_a_dongle(codex, second);
		print_events(codex, coder->id, "is debugging");
		usleep(codex->time_to_debug * 1000);
		print_events(codex, coder->id, "is refactoring");
		usleep(codex->time_to_refactor * 1000);
	}
	return (NULL);
}

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
	codex.simu_stopped = 0;
	if (!init_codex(&codex))
	{
		pthread_mutex_destroy(&codex.events_mutex);
		return (1);
	}
	codex.start_at = get_current_time();
	i = 0;
	while (i < codex.nb_coders)
	{
		codex.coders[i].last_compile_start = 0;
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
	free_codex(&codex);
	return (0);
}
