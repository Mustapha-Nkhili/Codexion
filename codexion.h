/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 15:54:31 by mn-khili          #+#    #+#             */
/*   Updated: 2026/08/02 17:23:56 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>

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
	t_request		waiters[2];
	int				waiters_len;
}	t_dongle;

typedef struct s_ticket_counter
{
	long long		next_ticket;
	pthread_mutex_t	lock;
}	t_ticket_counter;

typedef struct s_sim_state
{
	pthread_mutex_t	lock;
	pthread_cond_t	cond;
	int				stop;
	int				finished_count;
	int				number_of_coders;
}	t_sim_state;

typedef struct s_logger
{
	pthread_mutex_t	lock;
	long long		sim_start_time;
}	t_logger;

typedef struct s_coder_args
{
	t_coder				*coder;
	t_dongle			*dongles;
	t_ticket_counter	*ticket_counter;
	t_logger			*logger;
	t_sim_state			*sim_state;
}	t_coder_args;

typedef struct s_monitor_args
{
	t_coder			*coders;
	t_sim_state		*sim_state;
	t_logger		*logger;
}	t_monitor_args;

typedef struct s_sim_variables
{
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_t		*threads;
	t_coder_args	*coder_args;
	t_sim_state		*sim_state;
	t_monitor_args	*monitor_args;
	pthread_t		*monitor_thread;
	long long		sim_start_time;
}	t_sim_variables;

struct s_params	parse_args(int args_len, char *argv[]);
long			ft_atol(const char *str);
int				is_whitespace(char c);
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
void			init_dongles(t_dongle *dongles, int number_of_coders,
					long long sim_start);
void			log_state(t_logger *logger, int coder_id,
					t_coder_state state);
void			log_taken_dongle(t_logger *logger, int coder_id);
void			log_burnout(t_logger *logger, int coder_id);
int				get_left_dongle_index(int coder_id, int number_of_coders);
int				get_right_dongle_index(int coder_id);
struct timespec	ms_to_timespec(long long ms);
int				acquire_both_dongles(t_coder_args *args);
void			release_both_dongles(t_coder_args *args, int dongle_cooldown);
int				run_simulation(struct s_params *params,
					t_ticket_counter *ticket_counter);
void			init_sim(t_sim_state *sim, int number_of_coders);
int				is_sim_should_stop(t_sim_state *sim);
void			mark_coder_finished_sim(t_sim_state *sim);
void			mark_sim_burnout(t_sim_state *sim);
void			*coder_routine(void *a);
void			*monitor_routine_wrapper(void *arg);
int				handle_error(const char *err_source, const char *err_msg);
t_request		build_request(t_coder *coder, t_ticket_counter *counter);
long long		get_next_ticket(t_ticket_counter *counter);
int				free_sim_ressources(t_sim_variables *sim_vars,
					t_sim_state *sim_state, t_logger *logger);
void			init_sim_vars(t_sim_variables *sim_vars, t_sim_state *sim_state,
					t_monitor_args *monitor_args, pthread_t *monitor_thread);

#endif
