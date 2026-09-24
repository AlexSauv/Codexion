/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_handling.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/22 12:29:23 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/23 14:32:21 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	move_up(t_codex *codex, t_heap *heap, int i)
{
	int	before;

	while (i > 0)
	{
		before = (i - 1) / 2;
		if (!check_priority(codex, &heap->req[i], &heap->req[before]))
			break ;
		heap_swap(heap, i, before);
		i = before;
	}
	return (i);
}

static int	move_down(t_codex *codex, t_heap *heap, int i)
{
	int	left;
	int	right;
	int	choice;

	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		choice = i;
		if (left < heap->size && check_priority(codex, &heap->req[left],
				&heap->req[choice]))
			choice = left;
		if (right < heap->size && check_priority(codex, &heap->req[right],
				&heap->req[choice]))
			choice = right;
		if (choice == i)
			break ;
		heap_swap(heap, i, choice);
		i = choice;
	}
	return (i);
}

static int	find_coder_pos(t_heap *heap, int coder_id)
{
	int	i;

	i = 0;
	while (i < heap->size)
	{
		if (heap->req[i].coder_id == coder_id)
			return (i);
		i++;
	}
	return (-1);
}

void	remove_req(t_codex *codex, t_heap *heap, int coder_id)
{
	int	i;
	int	last;

	i = find_coder_pos(heap, coder_id);
	if (i == -1)
		return ;
	last = heap->size - 1;
	heap->req[i] = heap->req[last];
	heap->size--;
	if (i < heap->size)
	{
		i = move_up(codex, heap, i);
		move_down(codex, heap, i);
	}
}

void	update_req(t_codex *codex, t_heap *heap, t_req new_req)
{
	int	i;

	i = find_coder_pos(heap, new_req.coder_id);
	if (i == -1)
	{
		if (heap->size >= heap->capacity)
			return ;
		i = heap->size;
		heap->size++;
	}
	heap->req[i] = new_req;
	i = move_up(codex, heap, i);
	move_down(codex, heap, i);
}

int	get_coder_id(t_heap *heap)
{
	if (heap->size == 0)
		return (-1);
	return (heap->req[0].coder_id);
}
