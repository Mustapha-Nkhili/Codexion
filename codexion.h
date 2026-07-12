/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 15:54:31 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/12 10:29:40 by mn-khili         ###   ########.fr       */
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
	SCHED_FIFO,
	SCHED_EDF
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

struct s_params	parse_args(int args_len, char *argv[]);
long			ft_atol(const char *str);
int				is_whitespace(char c);

#endif
