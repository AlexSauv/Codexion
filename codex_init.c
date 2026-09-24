/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex_init.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 16:00:11 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/24 20:13:19 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	init_coders(t_codex *codex)
{
	long	i;

	i = 0;
	while (i < codex->nb_coders)
	{
		codex->coders[i].id = i + 1;
		codex->coders[i].nb_compile_done = 0;
		codex->coders[i].last_compile_start = 0;
		codex->coders[i].left_dongle = i;
		codex->coders[i].right_dongle = (i + 1) % codex->nb_coders;
		codex->coders[i].codex = codex;
		i++;
	}
	return (1);
}

int	generate_codex(t_codex *codex)
{
	long	i;

	i = 0;
	codex->mutexes = 0;
	codex->simu_stopped = 0;
	codex->start_at = get_current_time();
	codex->total_request = 0;
	while (i < codex->nb_coders)
	{
		if (pthread_mutex_init(&codex->dongles[i], NULL) != 0
			|| pthread_cond_init(&codex->condi[i], NULL) != 0)
		{
			fprintf(stderr, "[ERROR] Mutex / Condi failed for dongle %ld\n", i);
			free_codex(codex);
			return (0);
		}
		codex->dgl_heaps[i] = heap_create(codex->nb_coders);
		if (!codex->dgl_heaps)
		{
			fprintf(stderr, "[ERROR] Heap allocation failed for dngl %ld\n", i);
			codex->mutexes++;
			return (0);
		}
		codex->mutexes++;
		codex->dgl_cldwns[i] = 0;
		i++;
	}
	init_coders(codex);
	return (1);
}

int	init_codex(t_codex *codex)
{
	codex->dongles = NULL;
	codex->dgl_cldwns = NULL;
	codex->coders = NULL;
	codex->condi = NULL;
	codex->dgl_heaps = NULL;
	codex->mutexes = 0;
	codex->dongles = malloc(sizeof(pthread_mutex_t) * codex->nb_coders);
	codex->dgl_cldwns = malloc(sizeof(long) * codex->nb_coders);
	codex->coders = malloc(sizeof(t_coder) * codex->nb_coders);
	codex->condi = malloc(sizeof(pthread_cond_t) * codex->nb_coders);
	codex->dgl_heaps = malloc(sizeof(t_heap *) * codex->nb_coders);
	if (!codex->dongles || !codex->dgl_cldwns || !codex->coders
		|| !codex->condi || !codex->dgl_heaps)
	{
		fprintf(stderr, "[ERROR] Memory allocation failed.\n");
		free_codex(codex);
		return (0);
	}
	memset(codex->dongles, 0, sizeof(pthread_mutex_t) * codex->nb_coders);
	memset(codex->dgl_cldwns, 0, sizeof(long) * codex->nb_coders);
	memset(codex->coders, 0, sizeof(t_coder) * codex->nb_coders);
	memset(codex->condi, 0, sizeof(pthread_cond_t) * codex->nb_coders);
	memset(codex->dgl_heaps, 0, sizeof(t_heap *) * codex->nb_coders);
	if (!generate_codex(codex))
		return (0);
	return (1);
}
