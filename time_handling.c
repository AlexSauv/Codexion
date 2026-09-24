/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_handling.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 15:01:34 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/24 15:42:50 by alsauvan         ###   ########.fr       */
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

void	get_timeout(struct timespec *ts, long ms_from_now)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	ts->tv_sec = tv.tv_sec + (ms_from_now / 1000);
	ts->tv_nsec = (tv.tv_usec * 1000) + ((ms_from_now % 1000) * 1000000);
	if (ts->tv_nsec >= 1000000000)
	{
		ts->tv_nsec -= 1000000000;
		ts->tv_sec += 1;
	}
}
