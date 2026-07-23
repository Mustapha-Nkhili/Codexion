/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles_utils2.c                                    :+:     :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/23 06:30:30 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/23 06:55:16 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "codexion.h"

long long	get_next_ticket(t_ticket_counter *counter)
{
	long long	next_ticket;

	pthread_mutex_lock(&counter->lock);
	counter->next_ticket++;
	next_ticket = counter->next_ticket;
	pthread_mutex_unlock(&counter->lock);
	return (next_ticket);
}
