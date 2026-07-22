/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/19 21:41:56 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/22 04:36:37 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include <unistd.h>
#include "codexion.h"

static void	compile_phase(t_coder_args *arg, struct s_params *params)
{
	arg->coder->state = STATE_WAITING_FOR_DONGLES;
	acquire_both_dongles(arg->coder, arg->dongles, arg->ticket_counter,
		arg->logger);
	arg->coder->last_compile_start = get_timestamp_ms();
	arg->coder->state = STATE_COMPILING;
	log_state(arg->logger, arg->coder->id, STATE_COMPILING);
	usleep(params->time_to_compile * 1000);
	release_both_dongles(arg->coder, arg->dongles, params->dongle_cooldown);
	arg->coder->compile_count++;
	if (arg->coder->compile_count == params->number_of_compiles_required)
		mark_coder_finished_sim(arg->sim_state);
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
		compile_phase(arg, params);
		arg->coder->state = STATE_DEBUGGING;
		log_state(arg->logger, arg->coder->id, STATE_DEBUGGING);
		usleep(params->time_to_debug * 1000);
		arg->coder->state = STATE_REFACTORING;
		log_state(arg->logger, arg->coder->id, STATE_REFACTORING);
		usleep(params->time_to_refactor * 1000);
	}
	return (NULL);
}
