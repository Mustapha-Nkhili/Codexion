/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   priority_queue.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/09 10:56:14 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/28 02:13:08 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

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

int	heap_extract_min(t_request *min_heap, int *len, t_scheduler scheduler,
		t_request *out)
{
	t_request	min;
	t_request	last_item;

	if (*len == 0)
		return (1);
	last_item = min_heap[*len - 1];
	min = min_heap[0];
	*len -= 1;
	min_heap[0] = last_item;
	if (*len > 0)
		heapify(min_heap, *len, 0, scheduler);
	*out = min;
	return (0);
}
