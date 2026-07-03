/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mn-khili <mn-khili@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/02 17:32:39 by mn-khili          #+#    #+#             */
/*   Updated: 2026/07/02 18:19:02 by mn-khili         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	main(int argc, char *argv[])
{
	struct s_params	params;

	params.error = NULL;
	params = parse_args(argc, argv);
	if (params.error == NULL)
		printf("hello world");
	else
	{
		printf("%s\n", params.error);
		return (0);
	}
	return (0);
}
