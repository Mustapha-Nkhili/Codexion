/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 17:32:39 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/17 17:25:08 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/time.h>
#include <pthread.h>
#include "codexion.h"

long long	get_timestamp_ms(void)
{
	struct timeval	tv;
	long long		seconds;
	long long		microseconds;
	long long		milliseconds;

	gettimeofday(&tv, NULL);
	seconds = (long long)tv.tv_sec;
	microseconds = tv.tv_usec;
	milliseconds = (seconds * 1000) + (microseconds / 1000);
	return (milliseconds);
}

int	handle_error(const char *err_source, const char *err_msg)
{
	if (err_source != NULL && err_msg != NULL)
		fprintf(stderr, "Error (%s): %s\n", err_source, err_msg);
	else if (err_source != NULL && err_msg == NULL)
		fprintf(stderr, "Error in %s\n", err_source);
	else if (err_source == NULL && err_msg != NULL)
		fprintf(stderr, "Error: %s\n", err_msg);
	else
		fprintf(stderr, "Unexpected Error occured\n");
	return (1);
}

static int	run_simulation(struct s_params *params,
				t_ticket_counter *ticket_counter)
{
	t_coder		*coders;
	t_dongle	*dongles;
	int			result;

	coders = malloc(params->number_of_coders * sizeof(t_coder));
	if (coders == NULL)
		return (handle_error(NULL, "failed to allocate coders"));
	init_coders(coders, params, get_timestamp_ms());
	dongles = malloc(params->number_of_coders * sizeof(t_dongle));
	if (dongles == NULL)
		return (handle_error(NULL, "failed to allocate dongles"));
	result = init_dongles(dongles, params->number_of_coders,
			get_timestamp_ms());
	if (result != 0)
		return (handle_error(NULL, "initialisation of dongles failed"));
	acquire_dongle(&coders[0], &dongles[0], ticket_counter);
	printf("%d\n", dongles[0].waiters[0].coder_id);
	printf("%d\n", dongles[0].waiters_len);
	printf("%lld\n", dongles[0].waiters[0].deadline);
	printf("%lld\n", dongles[0].waiters[0].arrival_order);
	return (0);
}

int	main(int argc, char *argv[])
{
	struct s_params		params;
	const char			*err_msgs[6];
	t_ticket_counter	ticket_counter;

	pthread_mutex_init(&ticket_counter.lock, NULL);
	ticket_counter.next_ticket = 0;
	init_err_msgs(err_msgs);
	params = parse_args(argc, argv);
	if (params.err_code != ERR_NONE)
		return (handle_error(params.err_arg, err_msgs[params.err_code]));
	if (run_simulation(&params, &ticket_counter) != 0)
		return (1);
	return (0);
}
