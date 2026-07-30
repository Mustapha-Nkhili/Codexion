/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/18 13:51:46 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/30 04:20:15 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "codexion.h"

int	get_left_dongle_index(int coder_id, int number_of_coders)
{
	if (coder_id == 1)
		return (number_of_coders - 1);
	return (coder_id - 2);
}

int	get_right_dongle_index(int coder_id)
{
	return (coder_id - 1);
}

t_request	build_request(t_coder *coder, t_ticket_counter *counter)
{
	t_request	request;

	pthread_mutex_lock(&coder->lock);
	request.coder_id = coder->id;
	request.deadline = coder->last_compile_start
		+ coder->params->time_to_burnout;
	pthread_mutex_unlock(&coder->lock);
	request.arrival_order = get_next_ticket(counter);
	return (request);
}

static void	release_dongle(t_dongle *dongle, int dongle_cooldown)
{
	dongle->available = 1;
	dongle->free_at = get_timestamp_ms() + dongle_cooldown;
}

void	release_both_dongles(t_coder_args *args, int dongle_cooldown)
{
	int	left;
	int	right;

	left = get_left_dongle_index(args->coder->id,
			args->coder->params->number_of_coders);
	right = get_right_dongle_index(args->coder->id);
	pthread_mutex_lock(&args->sim_state->lock);
	release_dongle(&args->dongles[left], dongle_cooldown);
	release_dongle(&args->dongles[right], dongle_cooldown);
	pthread_cond_broadcast(&args->sim_state->cond);
	pthread_mutex_unlock(&args->sim_state->lock);
}
