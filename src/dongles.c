/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/16 19:04:17 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/23 06:59:40 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "codexion.h"

void	acquire_dongle(t_coder *coder, t_dongle *dongle,
			t_ticket_counter *counter, t_sim_state *sim_state)
{
	t_request	request;
	t_scheduler	scheduler;

	scheduler = coder->params->scheduler;
	request = build_request(coder, counter);
	pthread_mutex_lock(&dongle->lock);
	heap_insert(dongle->waiters, request, &dongle->waiters_len, scheduler);
	if (wait_for_turn(dongle, coder->id, sim_state))
	{
		pthread_mutex_unlock(&dongle->lock);
		return ;
	}
	dongle->available = 0;
	heap_extract_min(dongle->waiters, &dongle->waiters_len, scheduler,
		&request);
	pthread_mutex_unlock(&dongle->lock);
}

void	release_dongle(t_dongle *dongle, int dongle_cooldown)
{
	pthread_mutex_lock(&dongle->lock);
	dongle->available = 1;
	dongle->free_at = get_timestamp_ms() + dongle_cooldown;
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->lock);
}

static int	acquire_one_dongle(t_coder_args *args, int index)
{
	acquire_dongle(args->coder, &args->dongles[index],
		args->ticket_counter, args->sim_state);
	if (is_sim_should_stop(args->sim_state))
		return (1);
	log_taken_dongle(args->logger, args->coder->id);
	return (0);
}

void	acquire_both_dongles(t_coder_args *args)
{
	int	left;
	int	right;
	int	number_of_coders;

	number_of_coders = args->coder->params->number_of_coders;
	left = get_left_dongle_index(args->coder->id, number_of_coders);
	right = get_right_dongle_index(args->coder->id);
	if (left < right)
	{
		if (acquire_one_dongle(args, left))
			return ;
		acquire_one_dongle(args, right);
	}
	else
	{
		if (acquire_one_dongle(args, right))
			return ;
		acquire_one_dongle(args, left);
	}
}

void	release_both_dongles(t_coder *coder, t_dongle *dongles,
			int dongle_cooldown)
{
	int	left;
	int	right;
	int	number_of_coders;

	number_of_coders = coder->params->number_of_coders;
	left = get_left_dongle_index(coder->id, number_of_coders);
	right = get_right_dongle_index(coder->id);
	release_dongle(&dongles[left], dongle_cooldown);
	release_dongle(&dongles[right], dongle_cooldown);
}
