/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/18 13:51:46 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/23 06:58:10 by mn-khili         ###   ########.fr       */
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

struct timespec	ms_to_timespec(long long ms)
{
	struct timespec	ts;

	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	return (ts);
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

int	wait_for_turn(t_dongle *dongle, int coder_id,
				t_sim_state *sim_state)
{
	int				is_coder_min;
	struct timespec	deadline_ts;
	long long		now;
	long long		wake_at;

	is_coder_min = dongle->waiters[0].coder_id == coder_id;
	while ((!dongle->available || get_timestamp_ms() < dongle->free_at
			|| !is_coder_min) && !is_sim_should_stop(sim_state))
	{
		now = get_timestamp_ms();
		wake_at = dongle->free_at;
		if (wake_at <= now)
			wake_at = now + 5;
		deadline_ts = ms_to_timespec(wake_at);
		pthread_cond_timedwait(&dongle->cond, &dongle->lock, &deadline_ts);
		is_coder_min = (dongle->waiters[0].coder_id == coder_id);
	}
	return (is_sim_should_stop(sim_state));
}
