/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 13:33:40 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/21 15:52:09 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	get_priority_deadline(t_coder *coder, int dongle)
{
	t_codex		*codex;
	int			i;
	long		first_deadline;
	long		second_deadline;

	i = 0;
	codex = coder->codex;
	first_deadline = coder->last_compile_start + codex->time_to_burnout;
	while (i < codex->nb_coders)
	{
		if (codex->coders[i].id != coder->id)
		{
			if ((codex->coders[i].left_dongle == dongle)
				|| codex->coders[i].right_dongle == dongle)
			{
				pthread_mutex_lock(&codex->events_mutex);
				second_deadline = (codex->coders[i].last_compile_start
						+ codex->time_to_burnout);
				pthread_mutex_unlock(&codex->events_mutex);
				if (second_deadline < first_deadline)
					return (0);
			}
		}
		i++;
	}
	return (1);
}

int	first_to_request(t_coder *coder, int dongle)
{
	int			i;
	long		coder_req;
	long		second_req;
	t_codex		*codex;

	i = 0;
	codex = coder->codex;
    coder_req = coder->request;
	while (i < codex->nb_coders)
	{
		if (codex->coders[i].id != coder->id)
		{
            pthread_mutex_lock(&codex->events_mutex);
            second_req = codex->coders[i].request;
            pthread_mutex_unlock(&codex->events_mutex);
			if ((codex->coders[i].left_dongle == dongle
					|| codex->coders[i].right_dongle == dongle))
			{
				if (second_req > 0 && second_req < coder_req)
					return (0);
			}
		}
        i++;
	}
	return (1);
}
int	dongle_available(t_coder *coder, int dongle)
{
	t_codex *codex;
	long	curr_time;
	int		ready;

    ready = 0;
	codex = coder->codex;
	curr_time = get_current_time() - codex->start_at;
	if (curr_time < codex->dongle_cooldowns[dongle])
		return (0);
	if (strcmp(codex->scheduler, "fifo") == 0)
		ready = first_to_request(coder, dongle);
	else
		ready = get_priority_deadline(coder, dongle);
	return (ready);
}
