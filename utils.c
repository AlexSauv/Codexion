/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 13:33:40 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/24 17:03:23 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	check_priority(t_codex *cdx, t_req *frst, t_req *scnd)
{
	if (strcmp(cdx->scheduler, "fifo") == 0)
	{
		if (frst->req_time != scnd->req_time)
			return (frst->req_time < scnd->req_time);
		return (frst->coder_id < scnd->coder_id);
	}
	if (frst->deadline != scnd->deadline)
		return (frst->deadline < scnd->deadline);
	return (frst->coder_id < scnd->coder_id);
}

int	get_coder_id(t_heap *heap)
{
	if (heap->size == 0)
		return (-1);
	return (heap->req[0].coder_id);
}

int	check_coder_id(t_coder *coder, int dgl)
{
	t_codex	*codex;

	codex = coder->codex;
	return (get_coder_id(codex->dgl_heaps[dgl]) == coder->id);
}

int	dgl_is_free(t_coder *coder, int dongle)
{
	t_codex	*codex;
	long	curr_time;
	long	cooldown;

	codex = coder->codex;
	pthread_mutex_lock(&codex->events_mutex);
	cooldown = codex->dgl_cldwns[dongle];
	pthread_mutex_unlock(&codex->events_mutex);
	curr_time = get_current_time() - codex->start_at;
	if (curr_time < cooldown)
		return (0);
	return (1);
}

void	print_events(t_codex *cdx, int id, char *status)
{
	long	curr_time;

	pthread_mutex_lock(&cdx->events_mutex);
	if (strcmp(status, "burned out") == 0)
	{
		curr_time = get_current_time() - cdx->start_at;
		printf("%ld %d burned out\n", curr_time, id);
	}
	else if (!cdx->simu_stopped)
	{
		curr_time = get_current_time() - cdx->start_at;
		printf("%ld %d %s\n", curr_time, id, status);
	}
	pthread_mutex_unlock(&cdx->events_mutex);
}
