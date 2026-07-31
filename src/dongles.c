/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/16 19:04:17 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/31 10:14:22 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "codexion.h"

static int	can_acquire_both_dongles(t_coder *coder, t_dongle *dongles,
			int left, int right)
{
	long long	now;

	now = get_timestamp_ms();
	if (left == right)
		return (0);
	if (!dongles[left].available || !dongles[right].available)
		return (0);
	if (now < dongles[left].free_at || now < dongles[right].free_at)
		return (0);
	if (dongles[left].waiters_len == 0
		|| dongles[left].waiters[0].coder_id != coder->id)
		return (0);
	if (dongles[right].waiters_len == 0
		|| dongles[right].waiters[0].coder_id != coder->id)
		return (0);
	return (1);
}

static void	push_dongles(t_coder *coder, t_ticket_counter *counter,
			t_dongle *dongles)
{
	t_request	request;
	t_scheduler	scheduler;
	int			left;
	int			right;

	left = get_left_dongle_index(coder->id, coder->params->number_of_coders);
	right = get_right_dongle_index(coder->id);
	scheduler = coder->params->scheduler;
	request = build_request(coder, counter);
	heap_insert(dongles[left].waiters, request, &dongles[left].waiters_len,
		scheduler);
	heap_insert(dongles[right].waiters, request, &dongles[right].waiters_len,
		scheduler);
}

static void	extract_dongles(t_coder_args *args, int left, int right)
{
	t_request	request;
	t_scheduler	sch;

	sch = args->coder->params->scheduler;
	heap_extract_min(args->dongles[left].waiters,
		&args->dongles[left].waiters_len, sch, &request);
	heap_extract_min(args->dongles[right].waiters,
		&args->dongles[right].waiters_len, sch, &request);
	args->dongles[left].available = 0;
	args->dongles[right].available = 0;
	log_taken_dongle(args->logger, args->coder->id);
	log_taken_dongle(args->logger, args->coder->id);
}

static void	wait_for_dongles(t_coder_args *args, int left, int right,
			long long deadline)
{
	long long		now;
	long long		wake_at;
	struct timespec	deadline_ts;

	while (!can_acquire_both_dongles(args->coder, args->dongles, left, right)
		&& !args->sim_state->stop && get_timestamp_ms() < deadline)
	{
		now = get_timestamp_ms();
		wake_at = args->dongles[left].free_at;
		if (args->dongles[right].free_at > wake_at)
			wake_at = args->dongles[right].free_at;
		if (wake_at <= now)
			wake_at = now + 5;
		deadline_ts = ms_to_timespec(wake_at);
		pthread_cond_timedwait(&args->sim_state->cond,
			&args->sim_state->lock, &deadline_ts);
	}
}

int	acquire_both_dongles(t_coder_args *args)
{
	int			left;
	int			right;
	long long	deadline;

	left = get_left_dongle_index(args->coder->id,
			args->coder->params->number_of_coders);
	right = get_right_dongle_index(args->coder->id);
	pthread_mutex_lock(&args->sim_state->lock);
	deadline = args->coder->last_compile_start
		+ args->coder->params->time_to_burnout;
	push_dongles(args->coder, args->ticket_counter, args->dongles);
	wait_for_dongles(args, left, right, deadline);
	if (can_acquire_both_dongles(args->coder, args->dongles, left, right)
		&& !args->sim_state->stop && get_timestamp_ms() < deadline)
		extract_dongles(args, left, right);
	else
	{
		pthread_mutex_unlock(&args->sim_state->lock);
		return (1);
	}
	pthread_mutex_unlock(&args->sim_state->lock);
	return (0);
}
