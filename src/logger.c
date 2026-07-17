/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   logger.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/17 17:19:25 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/18 00:43:30 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <pthread.h>
#include "codexion.h"

static const char	*state_to_str(t_coder_state state)
{
	if (state == STATE_COMPILING)
		return ("is compiling");
	if (state == STATE_DEBUGGING)
		return ("is debugging");
	if (state == STATE_REFACTORING)
		return ("is refactoring");
	return ("is waiting for dongles");
}

void	log_state(pthread_mutex_t *logger_lock, int coder_id,
			t_coder_state state)
{
	pthread_mutex_lock(logger_lock);
	printf("%lld %d %s\n", get_timestamp_ms(), coder_id, state_to_str(state));
	pthread_mutex_unlock(logger_lock);
}
