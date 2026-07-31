/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/14 11:14:51 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/31 09:21:01 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "codexion.h"

void	init_err_msgs(const char *err_msgs[])
{
	err_msgs[0] = "no error";
	err_msgs[1] = "number of arguments must be exactly 8";
	err_msgs[2] = "must be a number";
	err_msgs[3] = "exceeds integer limits";
	err_msgs[4] = "must be a positive number";
	err_msgs[5] = "scheduler must be 'fifo' or 'edf'";
}

void	init_coders(t_coder *coders, struct s_params *params,
			long long sim_start)
{
	int	i;

	i = 0;
	while (i < params->number_of_coders)
	{
		coders[i].id = i + 1;
		coders[i].last_compile_start = sim_start;
		coders[i].state = STATE_WAITING_FOR_DONGLES;
		coders[i].compile_count = 0;
		coders[i].params = params;
		i++;
	}
}

void	init_dongles(t_dongle *dongles, int number_of_coders,
			long long sim_start)
{
	int	i;

	i = 0;
	while (i < number_of_coders)
	{
		dongles[i].id = i + 1;
		dongles[i].available = 1;
		dongles[i].free_at = sim_start;
		dongles[i].waiters_len = 0;
		i++;
	}
}
