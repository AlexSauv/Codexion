/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alsauvan <alsauvan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 15:01:43 by alsauvan          #+#    #+#             */
/*   Updated: 2026/09/18 15:25:38 by alsauvan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	scheduler_checker(char *str)
{
	if (strcmp(str, "fifo") == 0)
		return (1);
	if (strcmp(str, "edf") == 0)
		return (1);
	return (0);
}

static int	digits_checker(char *str)
{
	int		i;

	i = 0;
	if (!str || str[0] == '\0')
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static void	init_simu(t_codex *codex, char **argv)
{
	codex->nb_coders = atoi(argv[1]);
	codex->time_to_burnout = atoi(argv[2]);
	codex->time_to_compile = atoi(argv[3]);
	codex->time_to_debug = atoi(argv[4]);
	codex->time_to_refactor = atoi(argv[5]);
	codex->nb_comp_required = atoi(argv[6]);
	codex->cooldown_dngl = atoi(argv[7]);
	codex->scheduler = argv[8];
}

int	args_checker(int argc, char **argv, t_codex *codex)
{
	int	i;

	if (argc != 9)
	{
		fprintf(stderr, "[ERROR][PARSING] Invalid amount of arguments.\n");
		return (0);
	}
	i = 0;
	while (++i < 8)
	{
		if (!digits_checker(argv[i]))
		{
			fprintf(stderr, "[ERROR][PARSING] Invalid Format, "
				"Argument %d must be a positive digit.\n", i);
			return (0);
		}
	}
	if (!scheduler_checker(argv[8]))
	{
		fprintf(stderr, "[ERROR][PARSING] The scheduler must be"
			" either 'fifo' or 'edf'.\n");
		return (0);
	}
	init_simu(codex, argv);
	return (1);
}
