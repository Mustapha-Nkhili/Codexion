/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/16 19:04:17 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/20 18:24:31 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "codexion.h"

long long	get_next_ticket(t_ticket_counter *counter)
{
	long long	next_ticket;

	pthread_mutex_lock(&counter->lock);
	counter->next_ticket++;
	next_ticket = counter->next_ticket;
	pthread_mutex_unlock(&counter->lock);
	return (next_ticket);
}

void	acquire_dongle(t_coder *coder, t_dongle *dongle,
			t_ticket_counter *counter)
{
	t_request		request;
	t_scheduler		scheduler;
	int				is_coder_min;
	struct timespec	deadline_ts;

	scheduler = coder->params->scheduler;
	request.coder_id = coder->id;
	request.deadline = coder->last_compile_start
		+ coder->params->time_to_burnout;
	request.arrival_order = get_next_ticket(counter);
	pthread_mutex_lock(&dongle->lock);
	heap_insert(dongle->waiters, request, &dongle->waiters_len, scheduler);
	is_coder_min = (dongle->waiters[0].coder_id == coder->id);
	while (!dongle->available || get_timestamp_ms() < dongle->free_at
		|| !is_coder_min)
	{
		deadline_ts = ms_to_timespec(dongle->free_at);
		pthread_cond_timedwait(&dongle->cond, &dongle->lock, &deadline_ts);
		is_coder_min = (dongle->waiters[0].coder_id == coder->id);
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

void	acquire_both_dongles(t_coder *coder, t_dongle *dongles,
			t_ticket_counter *counter, pthread_mutex_t *logger_lock)
{
	int	left;
	int	right;
	int	number_of_coders;

	number_of_coders = coder->params->number_of_coders;
	left = get_left_dongle_index(coder->id, number_of_coders);
	right = get_right_dongle_index(coder->id);
	if (left < right)
	{
		acquire_dongle(coder, &dongles[left], counter);
		log_taken_dongle(logger_lock, coder->id);
		acquire_dongle(coder, &dongles[right], counter);
		log_taken_dongle(logger_lock, coder->id);
	}
	else
	{
		acquire_dongle(coder, &dongles[right], counter);
		log_taken_dongle(logger_lock, coder->id);
		acquire_dongle(coder, &dongles[left], counter);
		log_taken_dongle(logger_lock, coder->id);
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
