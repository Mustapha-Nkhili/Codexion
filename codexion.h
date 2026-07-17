/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 15:54:31 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/17 18:40:15 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <limits.h>
# include <string.h>

typedef enum e_err
{
	ERR_NONE = 0,
	ERR_ARGC,
	ERR_NOT_NUMBER,
	ERR_RANGE,
	ERR_NOT_POSITIVE,
	ERR_SCHEDULER
}	t_err;

typedef enum e_scheduler
{
	CODEXION_SCHED_FIFO,
	CODEXION_SCHED_EDF
}	t_scheduler;

struct s_params
{
	int			number_of_coders;
	int			time_to_burnout;
	int			time_to_compile;
	int			time_to_debug;
	int			time_to_refactor;
	int			number_of_compiles_required;
	int			dongle_cooldown;
	t_scheduler	scheduler;
	t_err		err_code;
	const char	*err_arg;
};

typedef enum s_coder_state
{
	STATE_COMPILING,
	STATE_DEBUGGING,
	STATE_REFACTORING,
	STATE_WAITING_FOR_DONGLES
}	t_coder_state;

typedef struct s_coder
{
	int				id;
	long long		last_compile_start;
	t_coder_state	state;
	int				compile_count;
	struct s_params	*params;
}	t_coder;

typedef struct s_request
{
	int			coder_id;
	long long	deadline;
	long long	arrival_order;
}	t_request;

typedef struct s_dongle
{
	int				id;
	int				available;
	long long		free_at;
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
	t_request		waiters[2];
	int				waiters_len;
}	t_dongle;

typedef struct s_ticket_counter
{
	long long		next_ticket;
	pthread_mutex_t	lock;
}	t_ticket_counter;

struct s_params	parse_args(int args_len, char *argv[]);
long			ft_atol(const char *str);
int				is_whitespace(char c);
void			build_heap(t_request arr[], int n, t_scheduler scheduler);
int				heap_insert(t_request *array, t_request new_req, int *len,
					t_scheduler scheduler);
int				heap_extract_min(t_request *min_heap, int *len,
					t_scheduler scheduler, t_request *out);
void			swap(t_request *a, t_request *b);
int				has_higher_priority(t_request a, t_request b,
					t_scheduler scheduler);
long long		get_timestamp_ms(void);
void			init_err_msgs(const char *err_msgs[]);
void			init_coders(t_coder *coders, struct s_params *params,
					long long sim_start);
int				init_dongles(t_dongle *dongles, int number_of_coders,
					long long sim_start);
void			destroy_dongles(t_dongle *dongles, int dongles_nbr);
void			acquire_dongle(t_coder *coder, t_dongle *dongle,
					t_ticket_counter *counter);
void	release_dongle(t_dongle *dongle, int dongle_cooldown);

#endif
