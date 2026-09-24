/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/24 17:04:50 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/24 17:05:58 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	free_dgl_resources(t_codex *codex)
{
	long	i;

	if (codex->dongles && codex->condi)
	{
		i = 0;
		while (i < codex->mutexes)
		{
			pthread_mutex_destroy(&codex->dongles[i]);
			pthread_cond_destroy(&codex->condi[i]);
			i++;
		}
		free(codex->dongles);
		free(codex->condi);
	}
	if (codex->dgl_heaps)
	{
		i = 0;
		while (i < codex->nb_coders)
		{
			if (codex->dgl_heaps[i])
				heap_freed(codex->dgl_heaps[i]);
			i++;
		}
		free(codex->dgl_heaps);
	}
}

void	free_codex(t_codex *codex)
{
	free_dgl_resources(codex);
	if (codex->dgl_cldwns)
		free(codex->dgl_cldwns);
	if (codex->coders)
		free(codex->coders);
	pthread_mutex_destroy(&codex->events_mutex);
}