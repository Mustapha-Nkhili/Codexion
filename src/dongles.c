/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/16 19:04:17 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/25 02:00:09 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "codexion.h"

static int	acquire_dongle(t_coder *coder, t_dongle *dongle,
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
		return (1);
	}
	dongle->available = 0;
	heap_extract_min(dongle->waiters, &dongle->waiters_len, scheduler,
		&request);
	pthread_mutex_unlock(&dongle->lock);
	return (0);
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
	int	dongle_cooldown;

	dongle_cooldown = args->coder->params->dongle_cooldown;
	if (acquire_dongle(args->coder, &args->dongles[index],
			args->ticket_counter, args->sim_state))
		return (1);
	if (is_sim_should_stop(args->sim_state))
	{
		release_dongle(&args->dongles[index], dongle_cooldown);
		return (1);
	}
	log_taken_dongle(args->logger, args->coder->id);
	return (0);
}

void	acquire_both_dongles(t_coder_args *args)
{
	int	left;
	int	right;
	int	number_of_coders;
	int	dongle_cooldown;

	dongle_cooldown = args->coder->params->dongle_cooldown;
	number_of_coders = args->coder->params->number_of_coders;
	left = get_left_dongle_index(args->coder->id, number_of_coders);
	right = get_right_dongle_index(args->coder->id);
	if (left < right)
	{
		if (acquire_one_dongle(args, left) == 1)
			return ;
		if (acquire_one_dongle(args, right) == 1)
			release_dongle(&args->dongles[left], dongle_cooldown);
	}
	else
	{
		if (acquire_one_dongle(args, right) == 1)
			return ;
		if (acquire_one_dongle(args, left) == 1)
			release_dongle(&args->dongles[right], dongle_cooldown);
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
