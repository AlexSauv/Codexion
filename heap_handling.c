/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_handling.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 12:29:23 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/22 17:11:05 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_heap	*heap_create(int capacity)
{
	t_heap	*heap;

	heap = malloc(sizeof(t_heap));
	if (!heap)
		return (NULL);
	heap->req = malloc(sizeof(t_req) * capacity);
	if (!heap->req)
	{
		free(heap);
		return (NULL);
	}
	heap->capacity = capacity;
	heap->size = 0;
	return (heap);
}

void	remove_coder_req(t_codex *codex, t_heap *heap, int coder_id)
{
	int	i;

	i = 0;
	pthread_mutex_lock(&codex->events_mutex);
	while (i < heap->size)
	{
		if (heap->req[i].coder_id == coder_id)
		{
			while (i < heap->size - 1)
			{
				heap->req[i] = heap->req[i + 1];
				i++;
			}
			heap->size--;
			break ;
		}
		i++;
	}
	pthread_mutex_unlock(&codex->events_mutex);
}

void 	update_req(t_heap *heap, int coder_id, long req, long deadline)
{
	int	i;
	
	i = 0;
	while (i < heap->size)
	{
		if (heap->req[i].coder_id == coder_id)
		{
			heap->req[i].req_time = req;
			heap->req[i].deadline = deadline;
			return ;
		}
		i++;
	}
	if (heap->size < heap->capacity)
	{
		heap->req[heap->size].coder_id = coder_id;
		heap->req[heap->size].req_time = req;
		heap->req[heap->size].deadline = deadline;
		heap->size++;
	}
}
