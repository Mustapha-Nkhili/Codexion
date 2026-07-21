/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/14 11:14:51 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/21 17:42:17 by mn-khili         ###   ########.fr       */
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
		pthread_mutex_init(&coders[i].lock, NULL);
		i++;
	}
}

int	init_dongles(t_dongle *dongles, int number_of_coders, long long sim_start)
{
	int	i;

	i = 0;
	while (i < number_of_coders)
	{
		dongles[i].id = i + 1;
		dongles[i].available = 1;
		dongles[i].free_at = sim_start;
		dongles[i].waiters_len = 0;
		if (pthread_mutex_init(&dongles[i].lock, NULL) != 0)
		{
			destroy_dongles(dongles, i);
			return (1);
		}
		if (pthread_cond_init(&dongles[i].cond, NULL) != 0)
		{
			pthread_mutex_destroy(&dongles[i].lock);
			destroy_dongles(dongles, i);
			return (1);
		}
		i++;
	}
	return (0);
}

void	destroy_dongles(t_dongle *dongles, int dongles_nbr)
{
	int	i;

	i = 0;
	while (i < dongles_nbr)
	{
		pthread_mutex_destroy(&dongles[i].lock);
		pthread_cond_destroy(&dongles[i].cond);
		i++;
	}
}
