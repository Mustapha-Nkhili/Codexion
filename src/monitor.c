/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/20 02:28:29 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/22 04:36:04 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <pthread.h>
#include "codexion.h"

static int	check_coder_burnout(t_coder *coder, t_sim_state *sim_state,
				t_logger *logger)
{
	long long	deadline;

	pthread_mutex_lock(&coder->lock);
	deadline = coder->last_compile_start + coder->params->time_to_burnout;
	if (get_timestamp_ms() >= deadline)
	{
		log_burnout(logger, coder->id);
		mark_sim_burnout(sim_state);
		pthread_mutex_unlock(&coder->lock);
		return (1);
	}
	pthread_mutex_unlock(&coder->lock);
	return (0);
}

static void	monitor_routine(t_coder *coders, t_sim_state *sim_state,
			t_logger *logger)
{
	int			i;
	int			number_of_coders;

	number_of_coders = coders[0].params->number_of_coders;
	while (1)
	{
		if (is_sim_should_stop(sim_state))
			break ;
		i = 0;
		while (i < number_of_coders)
		{
			if (check_coder_burnout(&coders[i], sim_state, logger))
				return ;
			i++;
		}
		usleep(1000);
	}
}

void	*monitor_routine_wrapper(void *arg)
{
	t_monitor_args	*m;

	m = (t_monitor_args *)arg;
	monitor_routine(m->coders, m->sim_state, m->logger);
	return (NULL);
}
