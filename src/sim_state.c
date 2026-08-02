/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_state.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/18 03:21:54 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/31 11:25:04 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "codexion.h"

int	free_sim_ressources(t_sim_variables *sim_vars,
		t_sim_state *sim_state, t_logger *logger)
{
	free(sim_vars->coders);
	free(sim_vars->dongles);
	free(sim_vars->threads);
	free(sim_vars->coder_args);
	if (sim_state != NULL)
	{
		pthread_mutex_destroy(&sim_state->lock);
		pthread_cond_destroy(&sim_state->cond);
	}
	if (logger != NULL)
		pthread_mutex_destroy(&logger->lock);
	return (1);
}

int	is_sim_should_stop(t_sim_state *sim)
{
	int	stop;

	pthread_mutex_lock(&sim->lock);
	stop = sim->stop;
	pthread_mutex_unlock(&sim->lock);
	return (stop);
}

void	mark_sim_burnout(t_sim_state *sim)
{
	pthread_mutex_lock(&sim->lock);
	sim->stop = 1;
	pthread_cond_broadcast(&sim->cond);
	pthread_mutex_unlock(&sim->lock);
}

void	mark_coder_finished_sim(t_sim_state *sim)
{
	pthread_mutex_lock(&sim->lock);
	sim->finished_count++;
	if (sim->finished_count == sim->number_of_coders)
	{
		sim->stop = 1;
		pthread_cond_broadcast(&sim->cond);
	}
	pthread_mutex_unlock(&sim->lock);
}
