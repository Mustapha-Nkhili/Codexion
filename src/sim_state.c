/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_state.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/18 03:21:54 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/29 06:44:10 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "codexion.h"

void	init_sim(t_sim_state *sim, int number_of_coders)
{
	pthread_mutex_init(&sim->lock, NULL);
	pthread_cond_init(&sim->cond, NULL);
	sim->stop = 0;
	sim->finished_count = 0;
	sim->number_of_coders = number_of_coders;
}

int	free_sim_ressources(t_sim_variables *sim_vars,
		t_sim_state *sim_state, t_logger *logger, int number_of_coders)
{
	if (sim_vars->dongles != NULL)
		destroy_dongles(sim_vars->dongles, number_of_coders);
	if (sim_vars->coders != NULL)
		destroy_coders(sim_vars->coders, number_of_coders);
	free(sim_vars->coders);
	free(sim_vars->dongles);
	free(sim_vars->threads);
	free(sim_vars->coder_args);
	if (sim_state != NULL)
		pthread_mutex_destroy(&sim_state->lock);
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
