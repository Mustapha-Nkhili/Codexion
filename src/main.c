/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 17:32:39 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/25 01:02:54 by mn-khili         ###   ########.fr       */
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
	{
		pthread_mutex_destroy(&ticket_counter.lock);
		return (handle_error(params.err_arg, err_msgs[params.err_code]));
	}
	if (run_simulation(&params, &ticket_counter) != 0)
	{
		pthread_mutex_destroy(&ticket_counter.lock);
		return (1);
	}
	pthread_mutex_destroy(&ticket_counter.lock);
	return (0);
}
