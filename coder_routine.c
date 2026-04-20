/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehdemir <mehdemir@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 13:24:17 by mehdemir          #+#    #+#             */
/*   Updated: 2026/04/20 13:24:20 by mehdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->id % 2 == 0)
		precise_usleep(1, coder->env);
	while (check_simulation_status(coder->env))
	{
		if (!acquire_dongles(coder))
			break ;
		coder_compile(coder);
		release_dongle(coder, coder->left_dongle);
		release_dongle(coder, coder->right_dongle);
		coder_debug(coder);
		coder_refactor(coder);
	}
	return (NULL);
}

void	coder_refactor(t_coder *coder)
{
	if (!check_simulation_status(coder->env))
		return ;
	log_status(coder, "is refactoring");
	precise_usleep(coder->env->time_to_refactor, coder->env);
}

void	coder_compile(t_coder *coder)
{
	if (!check_simulation_status(coder->env))
		return ;
	pthread_mutex_lock(&coder->state_mutex);
	coder->last_compile_start = get_time_ms();
	pthread_mutex_unlock(&coder->state_mutex);
	log_status(coder, "is compiling");
	precise_usleep(coder->env->time_to_compile, coder->env);
	pthread_mutex_lock(&coder->state_mutex);
	coder->compile_count++;
	pthread_mutex_unlock(&coder->state_mutex);
}

void	coder_debug(t_coder *coder)
{
	if (!check_simulation_status(coder->env))
		return ;
	log_status(coder, "is debugging");
	precise_usleep(coder->env->time_to_debug, coder->env);
}
