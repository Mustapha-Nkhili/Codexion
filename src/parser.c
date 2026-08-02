/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 18:11:19 by mn-khili          #+#    #+#             */
/*   Updated: 2026/08/02 17:24:40 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>
#include <limits.h>
#include "codexion.h"

static int	is_valid_number(const char *str)
{
	if (str == NULL || *str == '\0')
		return (0);
	while (*str && is_whitespace(*str))
		str++;
	if (*str == '-' || *str == '+')
		str++;
	if (*str == '\0')
		return (0);
	while (*str)
	{
		if (*str >= '0' && *str <= '9')
			str++;
		else
			return (0);
	}
	return (1);
}

static int	parse_nbr(const char *str_nbr, const char *arg_name,
		int *number, struct s_params *params)
{
	long	value;

	if (!is_valid_number(str_nbr))
	{
		params->err_code = ERR_NOT_NUMBER;
		params->err_arg = arg_name;
		return (0);
	}
	value = ft_atol(str_nbr);
	if (value > INT_MAX || value < INT_MIN)
	{
		params->err_code = ERR_RANGE;
		params->err_arg = arg_name;
		return (0);
	}
	if (value < 0)
	{
		params->err_code = ERR_NOT_POSITIVE;
		params->err_arg = arg_name;
		return (0);
	}
	*number = (int)value;
	return (1);
}

static int	validate_coders_nbr(const char *str_nbr, int *number,
		struct s_params *params)
{
	if (!parse_nbr(str_nbr, "number of coders", number, params))
		return (0);
	if (*number == 0)
	{
		params->err_code = ERR_NOT_POSITIVE;
		params->err_arg = "number of coders";
		return (0);
	}
	if (*number > 3000)
	{
		params->err_code = ERR_RANGE;
		params->err_arg = "number of coders";
		return (0);
	}
	return (1);
}

int	parse_numeric_args(char *argv[], struct s_params *params)
{
	int	number;

	if (!validate_coders_nbr(argv[1], &number, params))
		return (0);
	params->number_of_coders = number;
	if (!parse_nbr(argv[2], "time to burnout", &number, params))
		return (0);
	params->time_to_burnout = number;
	if (!parse_nbr(argv[3], "time to compile", &number, params))
		return (0);
	params->time_to_compile = number;
	if (!parse_nbr(argv[4], "time to debug", &number, params))
		return (0);
	params->time_to_debug = number;
	if (!parse_nbr(argv[5], "time to refactor", &number, params))
		return (0);
	params->time_to_refactor = number;
	if (!parse_nbr(argv[6], "number of compile required", &number, params))
		return (0);
	params->number_of_compiles_required = number;
	if (!parse_nbr(argv[7], "dongle cooldown", &number, params))
		return (0);
	params->dongle_cooldown = number;
	return (1);
}

struct s_params	parse_args(int args_len, char *argv[])
{
	struct s_params	params;

	params.err_code = ERR_NONE;
	params.err_arg = NULL;
	if (args_len != 9)
	{
		params.err_code = ERR_ARGC;
		return (params);
	}
	if (!parse_numeric_args(argv, &params))
		return (params);
	if (strcmp("fifo", argv[8]) == 0)
		params.scheduler = CODEXION_SCHED_FIFO;
	else if (strcmp("edf", argv[8]) == 0)
		params.scheduler = CODEXION_SCHED_EDF;
	else
	{
		params.err_code = ERR_SCHEDULER;
		params.err_arg = "scheduler";
		return (params);
	}
	return (params);
}
