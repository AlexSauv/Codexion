/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 16:00:11 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/17 17:29:56 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int generate_codex(t_codex *codex);

void free_codex(t_codex *codex)
{
    long i;

    if (codex->dongles)
    {
        i = 0;
        while (i < codex->nb_coders)
        {
            pthread_mutex_destroy(&codex->dongles[i]);
            i++;
        }
        free(codex->dongles);
    }
    if (codex->dongle_cooldowns)
        free(codex->dongle_cooldowns);
    if (codex->coders)
        free(codex->coders);
    pthread_mutex_destroy(&codex->log_mutex);
}
int init_codex(t_codex *codex)
{
    codex->dongles = malloc(sizeof(pthread_mutex_t) * codex->nb_coders);
    if(!codex->dongles)
    {
        fprintf(stderr, "[ERROR] Dongles memory allocation failed.");
        return (0);
    }
    codex->dongle_cooldowns = malloc(sizeof(long) * codex->nb_coders);
    if(!codex->dongle_cooldowns)
    {
        fprintf(stderr, "[ERROR] Dongle cooldowns memory allocation failed.");
        return (0);
    }
    codex->coders = malloc(sizeof(t_coder) * codex->nb_coders);
    if(!codex->coders)
    {
        fprintf(stderr, "[ERROR] Coders memory allocation failed.");
        return (0);
    }
    if (!generate_codex(codex))
        return (0);
    return (1);
}
int generate_codex(t_codex *codex)
{
    long i;

    i = 0;
    while (i < codex->nb_coders)
    {
        if (pthread_mutex_init(&codex->dongles[i], NULL) != 0)
        {
            fprintf(stderr, "[ERROR] Mutex init has failed for dongle %ld\n", i);
            return (0);
        }
        codex->dongle_cooldowns[i] = 0;
        i++;
    }
    
    i = 0;
    while (i < codex->nb_coders)
    {
        codex->coders[i].id = i + 1;
        codex->coders[i].nb_compile_done = 0;
        codex->coders[i].last_compile_start = 0;
        codex->coders[i].left_dongle = i;
        codex->coders[i].right_dongle = (i + 1) % codex->nb_coders;
        i++;
    }
    return (1);
}
