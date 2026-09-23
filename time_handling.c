/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_handling.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 15:01:34 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/23 14:42:33 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"
#include <sys/time.h>

long	get_current_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	print_events(t_codex *codex, int coder_id, char *status)
{
	long	curr_time;

	pthread_mutex_lock(&codex->events_mutex);
	if (strcmp(status, "burned out") == 0)
	{
		curr_time = get_current_time() - codex->start_at;
		printf("%ld %d burned out\n", curr_time, coder_id);
	}
	else if (!codex->simu_stopped)
	{
		curr_time = get_current_time() - codex->start_at;
		printf("%ld %d %s\n", curr_time, coder_id, status);
	}
	pthread_mutex_unlock(&codex->events_mutex);
}
