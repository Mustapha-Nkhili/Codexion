/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 17:32:39 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/04 23:19:34 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

int	main(int argc, char *argv[])
{
	struct s_params	params;
	const char		*err_msgs[6];

	init_err_msgs(err_msgs);
	params = parse_args(argc, argv);
	if (params.err_code == ERR_NONE)
		printf("hello world");
	else
	{
		if (params.err_arg)
			fprintf(stderr, "Error (%s): %s\n", params.err_arg,
				err_msgs[params.err_code]);
		else
			fprintf(stderr, "Error: %s\n", err_msgs[params.err_code]);
		return (1);
	}
	return (0);
}
