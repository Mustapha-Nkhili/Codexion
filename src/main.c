/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 17:32:39 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/12 10:28:29 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/time.h>
#include "codexion.h"

void	init_err_msgs(const char *err_msgs[])
{
	err_msgs[0] = "no error";
	err_msgs[1] = "number of arguments must be exactly 8";
	err_msgs[2] = "must be a number";
	err_msgs[3] = "exceeds integer limits";
	err_msgs[4] = "must be a positive number";
	err_msgs[5] = "scheduler must be 'fifo' or 'edf'";
}

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

void	init_coders(t_coder *coders, struct s_params *params,
			long long sim_start)
{
	int	i;

	i = 0;
	while (i < params->number_of_coders)
	{
		coders[i].id = i + 1;
		coders[i].last_compile_start = sim_start;
		coders[i].state = STATE_WAITING_FOR_DONGLES;
		coders[i].compile_count = 0;
		coders[i].params = params;
		i++;
	}
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
	struct s_params	params;
	const char		*err_msgs[6];
	t_coder			*coders;

	init_err_msgs(err_msgs);
	params = parse_args(argc, argv);
	if (params.err_code == ERR_NONE)
	{
		coders = malloc(params.number_of_coders * sizeof(t_coder));
		if (coders == NULL)
			return (handle_error(NULL, "failed to allocate coders"));
		init_coders(coders, &params, get_timestamp_ms());
	}
	else
		return (handle_error(params.err_arg, err_msgs[params.err_code]));
	return (0);
}
