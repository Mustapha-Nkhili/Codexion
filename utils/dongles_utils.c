/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/18 13:51:46 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/20 18:30:52 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	get_left_dongle_index(int coder_id, int number_of_coders)
{
	if (coder_id == 1)
		return (number_of_coders - 1);
	return (coder_id - 2);
}

int	get_right_dongle_index(int coder_id)
{
	return (coder_id - 1);
}

struct timespec	ms_to_timespec(long long ms)
{
	struct timespec	ts;

	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	return (ts);
}
