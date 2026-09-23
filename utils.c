/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 13:33:40 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/22 18:24:05 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int check_coder_priority(t_codex *codex, t_req *req_one, t_req *req_two)
{
	if (strcmp(codex->scheduler, "fifo") == 0)
	{
		if (req_one->req_time != req_two->req_time)
			return (req_one->req_time < req_two->req_time);
	}
	if (req_one->deadline != req_two->deadline)
		return (req_one->deadline < req_two->deadline);
	return (req_one->coder_id < req_two->coder_id);
}

int	get_priority(t_coder *coder, int dgl)
{
	t_codex		*codex;
	t_heap		*heap;
	int			i;
	long		coder_id;
	long		deadline;

	i = 1;
	codex = coder->codex;
	heap = codex->dongle_heaps[dgl];
	coder_id = heap->req[0].coder_id;
	deadline = heap->req[0].deadline;
	while (i < heap->size)
	{
		if (heap->req[i].deadline < deadline || (heap->req[i].deadline == deadline && heap->req[i].coder_id < coder_id))
		{
			coder_id = heap->req[i].coder_id;
			deadline = heap->req[i].deadline;
		}
		i++;
	}
	if (coder_id == coder->id)
		return (1);
	return (0);
}

int	first_to_req(t_coder *coder, int dongle)
{
	int			i;
	t_codex		*codex;
    t_heap		*heap;
	t_req		coder_req;
	int			coder_id;

	codex = coder->codex;
    heap = codex->dongle_heaps[dongle];
	if (!heap || heap->size == 0)
		return (0);
	i = 1;
	coder_req = heap->req[0];
	coder_id = heap->req[0].coder_id; 
	while (i < heap->size)
	{
		if (heap->req[i].req_time < coder_req.req_time || (heap->req[i].req_time == coder_req.req_time && heap->req[i].coder_id < coder_id))
		{
			coder_req = heap->req[i];
			coder_id = heap->req[i].coder_id;
		}
		i++;
	}
	return (coder_id == coder->id);
}

int	dongle_available(t_coder *coder, int dongle)
{
	t_codex *codex;
	long	curr_time;
	long	cooldown;

	codex = coder->codex;
	cooldown = codex->dongle_cooldowns[dongle];
	pthread_mutex_unlock(&codex->dongles[dongle]);
	curr_time = get_current_time() - codex->start_at;
	if (curr_time < cooldown)
		return (0);
	return (1);
}
