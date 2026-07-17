/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/16 19:04:17 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/17 17:45:37 by mn-khili         ###   ########.fr       */
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
	t_request	request;
	t_scheduler	scheduler;
	int			is_coder_min;

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
		pthread_cond_wait(&dongle->cond, &dongle->lock);
		is_coder_min = (dongle->waiters[0].coder_id == coder->id);
	}
	dongle->available = 0;
	heap_extract_min(dongle->waiters, &dongle->waiters_len, scheduler,
		&request);
	pthread_mutex_unlock(&dongle->lock);
}
