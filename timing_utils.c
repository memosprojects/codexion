/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   timing_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehdemir <mehdemir@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 13:25:33 by mehdemir          #+#    #+#             */
/*   Updated: 2026/04/20 13:25:34 by mehdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long long	get_time_ms(void)
{
	struct timeval	time;
	char			*err;

	if (gettimeofday(&time, NULL) == -1)
	{
		err = "gettimeofday() error\n";
		write(2, err, strlen(err));
	}
	return (((long long)time.tv_sec * 1000) + (time.tv_usec / 1000));
}

void	precise_usleep(long long duration, t_env *env)
{
	long long	start_time;
	long long	current_time;

	start_time = get_time_ms();
	current_time = get_time_ms();
	while (current_time - start_time < duration)
	{
		current_time = get_time_ms();
		pthread_mutex_lock(&env->print_mutex);
		if (!env->simulation_running)
		{
			pthread_mutex_unlock(&env->print_mutex);
			break ;
		}
		pthread_mutex_unlock(&env->print_mutex);
		usleep(500);
	}
}
