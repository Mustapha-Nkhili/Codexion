/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/18 13:51:46 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/30 03:04:01 by mn-khili         ###   ########.fr       */
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
