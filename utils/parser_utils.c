/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 11:28:40 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/12 16:01:28 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	is_whitespace(char c)
{
	if (c == ' ' || c == '\t')
		return (1);
	return (0);
}

static long	extract_number(const char *str, int sign, long limit)
{
	long	number;

	number = 0;
	while (*str)
	{
		if (*str >= '0' && *str <= '9')
		{
			if (number > limit / 10 || (number == limit / 10
					&& (*str - '0') > limit % 10))
			{
				if (sign == 1)
					return (LONG_MAX);
				else
					return (LONG_MIN);
			}
			number = (number * 10) + (*str - '0');
		}
		else
			break ;
		str++;
	}
	return (number * sign);
}

long	ft_atol(const char *str)
{
	long	number;
	long	limit;
	int		sign;

	number = 0;
	sign = 1;
	if (str == NULL || *str == '\0')
		return (number);
	while (*str && is_whitespace(*str))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	if (*str == '\0')
		return (number);
	limit = INT_MAX;
	if (sign == -1)
		limit = -(long)INT_MIN;
	return (extract_number(str, sign, limit));
}
