/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/16 19:04:17 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/29 06:51:20 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "codexion.h"

static void	release_dongle(t_dongle *dongle, int dongle_cooldown)
{
	dongle->available = 1;
	dongle->free_at = get_timestamp_ms() + dongle_cooldown;
}

void	release_both_dongles(t_coder_args *args,
			int dongle_cooldown)
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

static int	can_acquire_both_dongles(t_coder *coder, t_dongle *dongles, int left, int right)
{
    long long current_time;

    if (left == right)
        return (0);

    current_time = get_timestamp_ms();
    if (!dongles[left].available || !dongles[right].available)
        return (0);
    if (current_time < dongles[left].free_at || current_time < dongles[right].free_at)
        return (0);

    if (dongles[left].waiters_len == 0 || dongles[left].waiters[0].coder_id != coder->id)
        return (0);
    if (dongles[right].waiters_len == 0 || dongles[right].waiters[0].coder_id != coder->id)
        return (0);

    return (1);
}

static void	push_dongles_internal(t_coder *coder, t_ticket_counter *counter,
			t_dongle *dongles, int left, int right)
{
	t_request	request;
	t_scheduler	scheduler;

	scheduler = coder->params->scheduler;
	request = build_request(coder, counter);
	heap_insert(dongles[left].waiters, request, &dongles[left].waiters_len,
		scheduler);
	heap_insert(dongles[right].waiters, request, &dongles[right].waiters_len,
		scheduler);
}

int	acquire_both_dongles(t_coder_args *args)
{
    int left;
    int right;
    int number_of_coders;
    long long now;
    long long wake_at;
	long long deadline;
    struct timespec deadline_ts;

    number_of_coders = args->coder->params->number_of_coders;
    left = get_left_dongle_index(args->coder->id, number_of_coders);
    right = get_right_dongle_index(args->coder->id);

	pthread_mutex_lock(&args->coder->lock);
	deadline = args->coder->last_compile_start
		+ args->coder->params->time_to_burnout;
	pthread_mutex_unlock(&args->coder->lock);

	pthread_mutex_lock(&args->sim_state->lock);

    push_dongles_internal(args->coder, args->ticket_counter, args->dongles, left, right);

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
                               &args->sim_state->lock, 
                               &deadline_ts);
    }

	if (can_acquire_both_dongles(args->coder, args->dongles, left, right)
		&& !args->sim_state->stop && get_timestamp_ms() < deadline)
    {
		t_request	request;

		heap_extract_min(args->dongles[left].waiters, &args->dongles[left].waiters_len, args->coder->params->scheduler,
			&request);
		heap_extract_min(args->dongles[right].waiters, &args->dongles[right].waiters_len, args->coder->params->scheduler,
			&request);

        args->dongles[left].available = 0;
        args->dongles[right].available = 0;
		log_taken_dongle(args->logger, args->coder->id);
		log_taken_dongle(args->logger, args->coder->id);
    }
	else
    {
		pthread_mutex_unlock(&args->sim_state->lock);
		return (1);
	}
    pthread_mutex_unlock(&args->sim_state->lock);
	return (0);
}
