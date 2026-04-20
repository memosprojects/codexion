/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehdemir <mehdemir@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 13:25:00 by mehdemir          #+#    #+#             */
/*   Updated: 2026/04/20 13:25:02 by mehdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	*monitor_routine(void *arg)
{
	t_env	*env;

	env = (t_env *)arg;
	while (1)
	{
		if (check_burnout(env) || check_deployment(env))
			break ;
		usleep(MONITOR_INTERVAL);
	}
	return (arg);
}

int	check_burnout(t_env *env)
{
	int			i;
	long long	timestamp;

	i = 0;
	timestamp = 0;
	while (i < env->num_coders)
	{
		if (burnout_flag(&env->coders[i], env) == 1)
		{
			timestamp = get_time_ms() - env->start_time;
			pthread_mutex_lock(&env->print_mutex);
			env->simulation_running = 0;
			printf("%lld %d burned out\n", timestamp, env->coders[i].id);
			pthread_mutex_unlock(&env->print_mutex);
			return (1);
		}
		i++;
	}
	return (0);
}

int	burnout_flag(t_coder *coder, t_env *env)
{
	pthread_mutex_lock(&coder->state_mutex);
	if ((get_time_ms() - coder->last_compile_start) >= env->time_to_burnout)
	{
		pthread_mutex_unlock(&coder->state_mutex);
		return (1);
	}
	pthread_mutex_unlock(&coder->state_mutex);
	return (0);
}

int	check_deployment(t_env *env)
{
	int	i;
	int	finished_coders;

	i = 0;
	finished_coders = 0;
	if (env->num_compiles_req == -1)
		return (0);
	while (i < env->num_coders)
	{
		pthread_mutex_lock(&env->coders[i].state_mutex);
		if (env->coders[i].compile_count >= env->num_compiles_req)
			finished_coders++;
		pthread_mutex_unlock(&env->coders[i].state_mutex);
		i++;
	}
	if (finished_coders == env->num_coders)
	{
		pthread_mutex_lock(&env->print_mutex);
		env->simulation_running = 0;
		pthread_mutex_unlock(&env->print_mutex);
		return (1);
	}
	return (0);
}
