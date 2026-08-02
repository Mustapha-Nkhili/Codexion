/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 21:41:56 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/31 09:58:35 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <unistd.h>
#include "codexion.h"

static int	compile_phase(t_coder_args *arg, struct s_params *params)
{
	int	finished;

	arg->coder->state = STATE_WAITING_FOR_DONGLES;
	if (is_sim_should_stop(arg->sim_state))
		return (1);
	if (acquire_both_dongles(arg))
		return (1);
	pthread_mutex_lock(&arg->sim_state->lock);
	arg->coder->last_compile_start = get_timestamp_ms();
	pthread_mutex_unlock(&arg->sim_state->lock);
	arg->coder->state = STATE_COMPILING;
	log_state(arg->logger, arg->coder->id, STATE_COMPILING);
	usleep(params->time_to_compile * 1000);
	release_both_dongles(arg, params->dongle_cooldown);
	pthread_mutex_lock(&arg->sim_state->lock);
	arg->coder->compile_count++;
	finished = arg->coder->compile_count
		== params->number_of_compiles_required;
	pthread_mutex_unlock(&arg->sim_state->lock);
	if (finished)
		mark_coder_finished_sim(arg->sim_state);
	return (0);
}

void	*coder_routine(void *a)
{
	struct s_params	*params;
	t_coder_args	*arg;

	arg = (t_coder_args *)a;
	params = arg->coder->params;
	while (!is_sim_should_stop(arg->sim_state))
	{
		if (arg->coder->compile_count >= params->number_of_compiles_required)
			break ;
		if (compile_phase(arg, params))
			break ;
		arg->coder->state = STATE_DEBUGGING;
		if (is_sim_should_stop(arg->sim_state))
			break ;
		log_state(arg->logger, arg->coder->id, STATE_DEBUGGING);
		usleep(params->time_to_debug * 1000);
		arg->coder->state = STATE_REFACTORING;
		if (is_sim_should_stop(arg->sim_state))
			break ;
		log_state(arg->logger, arg->coder->id, STATE_REFACTORING);
		usleep(params->time_to_refactor * 1000);
	}
	return (NULL);
}
