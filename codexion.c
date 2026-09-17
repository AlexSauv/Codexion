/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 16:50:44 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/17 17:26:37 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void get_a_dongle(t_coder *coder, int dongle)
{
    t_codex *codex;
    long    curr_time;
    long    wait_time;
    
    codex = coder->codex;
    while (!codex->simu_stopped)
    {
        pthread_mutex_lock(&codex->dongles[dongle]);
        curr_time = get_current_time() - codex->start_at;
        
        if (curr_time >= codex->dongle_cooldowns[dongle])
        {
            print_log(codex, coder->id, "has_taken a dongle");
            break;
        }
        wait_time = codex->dongle_cooldowns[dongle] - curr_time;
        pthread_mutex_unlock(&codex->dongles[dongle]);

        if (wait_time > 0)
            usleep(wait_time * 1000);
    }
}

void drop_a_dongle(t_codex *codex, int dongle)
{
    codex->dongle_cooldowns[dongle] = (get_current_time() - codex->start_at) + codex->cooldown_dngl;
    pthread_mutex_unlock(&codex->dongles[dongle]);
}

void *main_coder_events(void *arg)
{
    t_coder *coder;
    t_codex *codex;
    int     first;
    int     second;

    coder = (t_coder *)arg;
    codex = coder->codex;
    
    coder->last_compile_start = get_current_time() - codex->start_at;
    while (!codex->simu_stopped)
    {
        if(coder->left_dongle < coder->right_dongle)
        {
            first = coder->left_dongle;
            second = coder->right_dongle;
        }
        else
        {
            first = coder->right_dongle;
            second = coder->left_dongle;
        }
        get_a_dongle(coder, first);
        get_a_dongle(coder, second);

        coder->last_compile_start = get_current_time() - codex->start_at;
        print_log(codex, coder->id, "is_compiling");
        usleep(codex->time_to_compile * 1000);
        coder->nb_compile_done++;

        drop_a_dongle(codex, first);
        drop_a_dongle(codex, second);

        print_log(codex, coder->id, "is debugging");
        usleep(codex->time_to_debug * 1000);

        print_log(codex, coder->id, "is refactoring");
        usleep(codex->time_to_refactor * 1000);
    }
    return (NULL);
}