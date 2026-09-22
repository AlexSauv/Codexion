/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/20 13:33:40 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/22 16:11:12 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	get_priority_deadline(t_coder *coder, int dgl)
{
	t_codex		*codex;
	t_heap		*heap;
	int			i;
	long		coder_id;
	long		deadline;

	i = 1;
	codex = coder->codex;
	heap = &codex->dongle_heaps[dgl];
	coder_id = heap->req[0].coder_id;
	deadline = heap->req[0].deadline;
	while (i < heap->size)
	{
		if (heap->req[i].deadline < deadline 
			|| (heap->req[i].deadline == deadline
				&& heap->req[i].coder_id < coder_id))
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

int	first_to_request(t_coder *coder, int dongle)
{
	int			i;
	t_codex		*codex;
    t_heap		*heap;
	t_req		coder_req;
	int			coder_id;

	codex = coder->codex;
    heap = &codex->dongle_heaps[dongle];
	if (!heap || heap->size == 0)
	return (0);
	i = 1;
	coder_req = heap->req[0];
	coder_id = heap->req[0].coder_id; 
	while (i < heap->size)
	{
		if (heap->req[i].req_time < coder_req.req_time 
			|| (heap->req[i].req_time == coder_req.req_time 
				&& heap->req[i].coder_id < coder_id))
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
