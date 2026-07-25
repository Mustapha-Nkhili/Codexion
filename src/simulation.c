/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/21 19:32:28 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/25 01:19:13 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <stdlib.h>
#include "codexion.h"

static int	allocate_resources(struct s_params *params,
				t_sim_variables *sim_vars, t_logger *logger)
{
	sim_vars->coders = malloc(params->number_of_coders * sizeof(t_coder));
	if (sim_vars->coders == NULL)
		return (handle_error(NULL, "failed to allocate coders"));
	logger->sim_start_time = get_timestamp_ms();
	init_coders(sim_vars->coders, params, logger->sim_start_time);
	sim_vars->dongles = malloc(params->number_of_coders * sizeof(t_dongle));
	if (sim_vars->dongles == NULL)
		return (handle_error(NULL, "failed to allocate dongles"));
	if (init_dongles(sim_vars->dongles, params->number_of_coders,
			get_timestamp_ms()) != 0)
		return (handle_error(NULL, "initialisation of dongles failed"));
	sim_vars->threads = malloc(params->number_of_coders * sizeof(pthread_t));
	if (sim_vars->threads == NULL)
		return (handle_error(NULL, "failed to allocate coders threads"));
	return (0);
}

static int	setup_simulation(struct s_params *params, t_sim_variables *sim_vars,
				t_sim_state *sim_state, t_logger *logger)
{
	int	result;

	result = allocate_resources(params, sim_vars, logger);
	if (result != 0)
		return (1);
	init_sim(sim_state, params->number_of_coders);
	return (0);
}

static void	create_threads(struct s_params *params, t_ticket_counter *counter,
				t_sim_variables *sim_vars, t_logger *logger)
{
	int	i;

	i = 0;
	while (i < params->number_of_coders)
	{
		sim_vars->coder_args[i].coder = &sim_vars->coders[i];
		sim_vars->coder_args[i].dongles = sim_vars->dongles;
		sim_vars->coder_args[i].ticket_counter = counter;
		sim_vars->coder_args[i].logger = logger;
		sim_vars->coder_args[i].sim_state = sim_vars->sim_state;
		pthread_create(&sim_vars->threads[i], NULL, coder_routine,
			&sim_vars->coder_args[i]);
		i++;
	}
	sim_vars->monitor_args->coders = sim_vars->coders;
	sim_vars->monitor_args->sim_state = sim_vars->sim_state;
	sim_vars->monitor_args->logger = logger;
	pthread_create(sim_vars->monitor_thread, NULL, monitor_routine_wrapper,
		sim_vars->monitor_args);
}

static void	join_threads(int number_of_coders, pthread_t *coders_threads,
				pthread_t monitor_thread)
{
	int	i;

	i = 0;
	while (i < number_of_coders)
	{
		pthread_join(coders_threads[i], NULL);
		i++;
	}
	pthread_join(monitor_thread, NULL);
}

int	run_simulation(struct s_params *params, t_ticket_counter *ticket_counter)
{
	t_sim_variables			sim_vars;
	t_logger				logger;
	t_sim_state				sim_state;
	t_monitor_args			monitor_args;
	pthread_t				monitor_thread;

	sim_vars.sim_state = &sim_state;
	sim_vars.monitor_args = &monitor_args;
	sim_vars.monitor_thread = &monitor_thread;
	sim_vars.coders = NULL;
	sim_vars.dongles = NULL;
	sim_vars.threads = NULL;
	sim_vars.coder_args = NULL;
	pthread_mutex_init(&logger.lock, NULL);
	if (setup_simulation(params, &sim_vars, &sim_state, &logger) != 0)
		return  (free_sim_ressources(&sim_vars, NULL, &logger, params->number_of_coders));
	sim_vars.coder_args = malloc(
			params->number_of_coders * sizeof(t_coder_args));
	if (sim_vars.coder_args == NULL)
	{
		free_sim_ressources(&sim_vars, &sim_state, &logger, params->number_of_coders);
		return (handle_error(NULL, "failed to allocate coders args"));
	}
	create_threads(params, ticket_counter, &sim_vars, &logger);
	join_threads(params->number_of_coders, sim_vars.threads, monitor_thread);
	destroy_coders(sim_vars.coders, params->number_of_coders);
	destroy_dongles(sim_vars.dongles, params->number_of_coders);
	free_sim_ressources(&sim_vars, &sim_state, &logger, params->number_of_coders);
	return (0);
}
