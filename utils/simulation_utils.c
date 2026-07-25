/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/25 06:17:48 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/25 07:18:45 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <pthread.h>
#include "codexion.h"

void	init_sim_vars(t_sim_variables *sim_vars, t_sim_state *sim_state,
			t_monitor_args *monitor_args, pthread_t *monitor_thread)
{
	sim_vars->sim_state = sim_state;
	sim_vars->monitor_args = monitor_args;
	sim_vars->monitor_thread = monitor_thread;
	sim_vars->coders = NULL;
	sim_vars->dongles = NULL;
	sim_vars->threads = NULL;
	sim_vars->coder_args = NULL;
}
