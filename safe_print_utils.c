/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   safe_print_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehdemir <mehdemir@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 13:25:19 by mehdemir          #+#    #+#             */
/*   Updated: 2026/04/20 13:25:21 by mehdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_status(t_coder *coder, char *status)
{
	long long	timestamp;

	pthread_mutex_lock(&coder->env->print_mutex);
	if (!coder->env->simulation_running)
	{
		pthread_mutex_unlock(&coder->env->print_mutex);
		return ;
	}
	timestamp = get_time_ms() - coder->env->start_time;
	printf("%lld %d %s\n", timestamp, coder->id, status);
	pthread_mutex_unlock(&coder->env->print_mutex);
}

void	debug_coders(t_env *env)
{
	int	i;

	i = 0;
	while (i < env->num_coders)
	{
		printf("--- Coder ID: %d ---\n", env->coders[i].id);
		printf("Compile Count:      %d\n", env->coders[i].compile_count);
		printf("Last Compile Start: %lld\n", env->coders[i].last_compile_start);
		printf("Burnout Deadline:   %lld\n", 
			env->coders[i].last_compile_start + env->time_to_burnout);
		printf("\n");
		i++;
	}
}
