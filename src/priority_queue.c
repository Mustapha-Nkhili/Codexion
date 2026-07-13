/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   priority_queue.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 10:56:14 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/13 15:56:18 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	swap(t_request *a, t_request *b)
{
	t_request	temp;

	temp = *a;
	*a = *b;
	*b = temp;
}

static int	has_higher_priority(t_request a, t_request b, t_scheduler scheduler)
{
	if (scheduler == SCHED_FIFO)
		return (a.arrival_order < b.arrival_order);
	if (a.deadline != b.deadline)
		return (a.deadline < b.deadline);
	return (a.arrival_order < b.arrival_order);
}

static void	heapify(t_request array[], int len, int i, t_scheduler scheduler)
{
	int	smallest;
	int	left;
	int	right;

	smallest = i;
	while (1)
	{
		left = 2 * i + 1;
		right = 2 * i + 2;
		if (left < len && has_higher_priority(array[left], array[smallest],
				scheduler))
			smallest = left;
		if (right < len && has_higher_priority(array[right], array[smallest],
				scheduler))
			smallest = right;
		if (smallest == i)
			break ;
		swap(&array[i], &array[smallest]);
		i = smallest;
	}
}

void	build_heap(t_request arr[], int n, t_scheduler scheduler)
{
	int	i;

	i = (n / 2) - 1;
	while (i >= 0)
	{
		heapify(arr, n, i, scheduler);
		i--;
	}
}

int	heap_insert(t_request *array, t_request new_req, int *len,
		t_scheduler scheduler)
{
	int			nbr_index;
	int			parent;

	if (*len > 1)
		return (1);
	array[*len] = new_req;
	nbr_index = *len;
	while (nbr_index != 0)
	{
		parent = (nbr_index - 1) / 2;
		if (has_higher_priority(array[nbr_index], array[parent], scheduler))
		{
			swap(&array[nbr_index], &array[parent]);
			nbr_index = parent;
		}
		else
			break ;
	}
	*len += 1;
	return (0);
}
