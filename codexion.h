/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/03 15:54:31 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/03 16:03:36 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <stdio.h>
# include <stdlib.h>
# include <limits.h>

struct s_params
{
	int			number_of_coders;
	const char	*error;
};

struct s_params	parse_args(int args_len, char *argv[]);
long			ft_atol(const char *str);
int				is_whitespace(char c);

#endif
