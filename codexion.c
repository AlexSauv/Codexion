/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 16:50:44 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/18 14:44:17 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	drop_a_dongle(t_codex *codex, int dongle)
{
	codex->dongle_cooldowns[dongle] = ((get_current_time() - codex->start_at)
			+ codex->cooldown_dngl);
	pthread_mutex_unlock(&codex->dongles[dongle]);
}

void	*coder_events(void *arg)
{
	t_coder		*coder;
	t_codex		*codex;
	int			first;
	int			second;

	coder = (t_coder *) arg;
	codex = coder->codex;
	coder->last_compile_start = get_current_time() - codex->start_at;
	while (!codex->simu_stopped)
	{
		if (coder->left_dongle < coder->right_dongle)
		{
			first = coder->left_dongle;
			second = coder->right_dongle;
		}
		else
		{
			first = coder->right_dongle;
			second = coder->left_dongle;
		}
		get_a_dongle(coder, first);
		get_a_dongle(coder, second);
		coder->last_compile_start = get_current_time() - codex->start_at;
		print_events(codex, coder->id, "is compiling");
		usleep(codex->time_to_compile * 1000);
		coder->nb_compile_done++;
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
