/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 13:33:40 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/23 13:52:14 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_coder_priority(t_codex *codex, t_req *first, t_req *scnd)
{
	if (strcmp(codex->scheduler, "fifo") == 0)
	{
		if (first->req_time != scnd->req_time)
			return (first->req_time < scnd->req_time);
		return (first->coder_id < scnd->coder_id);
	}
	if (first->deadline != scnd->deadline)
		return (first->deadline < scnd->deadline);
	return (first->coder_id < scnd->coder_id);
}

int	check_scheduler(t_coder *coder, int dgl)
{
	t_codex	*codex;
	int		coder_id;

	codex = coder->codex;
	coder_id = get_coder_id(codex->dongle_heaps[dgl]);
	return (coder_id);
}

int	dongle_available(t_coder *coder, int dongle)
{
	t_codex	*codex;
	long	curr_time;
	long	cooldown;

	codex = coder->codex;
	cooldown = codex->dgl_cldwns[dongle];
	pthread_mutex_unlock(&codex->dongles[dongle]);
	curr_time = get_current_time() - codex->start_at;
	if (curr_time < cooldown)
		return (0);
	return (1);
}
