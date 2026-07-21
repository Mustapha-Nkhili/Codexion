/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 17:32:39 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/21 17:47:08 by mn-khili         ###   ########.fr       */
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
	t_sim_state	sim_state;
	pthread_t	*coders_threads;
	t_coder_args	*coder_args;
	pthread_mutex_t	logger_lock;
	pthread_t	monitor_thread;
	t_monitor_args	monitor_args;

	pthread_mutex_init(&logger_lock, NULL);
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
	init_sim(&sim_state, params->number_of_coders);
	coders_threads = malloc(params->number_of_coders * sizeof(pthread_t));
	if (coders_threads == NULL)
		return (handle_error(NULL, "failed to allocate coders threads"));
	coder_args = malloc(params->number_of_coders * sizeof(t_coder_args));
	if (coder_args == NULL)
		return (handle_error(NULL, "failed to allocate coders args"));
	int i = 0;
	while (i < params->number_of_coders)
	{
		coder_args[i].coder = &coders[i];
		coder_args[i].dongles = dongles;
		coder_args[i].ticket_counter = ticket_counter;
		coder_args[i].logger_lock = &logger_lock;
		coder_args[i].sim_state = &sim_state;
		pthread_create(&coders_threads[i], NULL, coder_routine, &coder_args[i]);
		i++;
	}
	monitor_args.coders = coders;
	monitor_args.sim_state = &sim_state;
	monitor_args.logger_lock = &logger_lock;
	pthread_create(&monitor_thread, NULL, monitor_routine_wrapper, &monitor_args);
	i = 0;
	while (i < params->number_of_coders)
	{
		pthread_join(coders_threads[i], NULL);
		i++;
	}
	pthread_join(monitor_thread, NULL);
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
