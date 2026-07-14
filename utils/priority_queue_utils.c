/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   priority_queue_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/13 16:43:35 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/13 16:46:30 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	swap(t_request *a, t_request *b)
{
	t_request	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

int	has_higher_priority(t_request a, t_request b, t_scheduler scheduler)
{
	if (scheduler == SCHED_FIFO)
		return (a.arrival_order < b.arrival_order);
	if (a.deadline != b.deadline)
		return (a.deadline < b.deadline);
	return (a.arrival_order < b.arrival_order);
}
