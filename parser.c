/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 18:11:19 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/03 12:07:34 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

static struct s_params	validate_coders_nbr(long num, struct s_params params)
{
	char	*e;

	if (num > INT_MAX || num < INT_MIN)
	{
		e = "Error (number of coders): number exceeds integer limits";
		params.error = e;
		return (params);
	}
	if (num <= 0)
	{
		e = "Error (number of coders): it must be a positive number";
		params.error = e;
		return (params);
	}
	params.number_of_coders = num;
	return (params);
}

struct s_params	parse_args(int args_len, char *argv[])
{
	struct s_params	params;
	long			number_of_coders;

	params.error = NULL;
	if (args_len == 9)
	{
		if (is_valid_number(argv[1]))
		{
			number_of_coders = ft_atol(argv[1]);
			params = validate_coders_nbr(number_of_coders, params);
		}
		else
		{
			params.error = "Error (number of coders): it must be a number";
			return (params);
		}
	}
	else
	{
		params.error = "Error: number of args must be exactly 8";
		return (params);
	}
	return (params);
}
