/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder_routine_2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehdemir <mehdemir@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 13:23:32 by mehdemir          #+#    #+#             */
/*   Updated: 2026/04/20 13:23:35 by mehdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	take_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	add_to_queue(dongle, coder);
	while (check_simulation_status(coder->env) && 
		(dongle->owner_id != 0 || !is_my_turn(dongle, coder)))
	{
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}
	if (check_simulation_status(coder->env))
	{
		dongle->owner_id = coder->id;
		log_status(coder, "has taken a dongle");
	}
	pthread_mutex_unlock(&dongle->mutex);
}

void	release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->owner_id = 0;
	dongle->last_released_at = get_time_ms();
	remove_from_queue(dongle, coder->id);
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

int	acquire_dongles(t_coder *coder)
{
	if (coder->left_dongle == coder->right_dongle)
	{
		take_dongle(coder, coder->left_dongle);
		while (check_simulation_status(coder->env))
			precise_usleep(1, coder->env);
		return (0);
	}
	take_dongle(coder, coder->left_dongle);
	if (!check_simulation_status(coder->env))
		return (0);
	take_dongle(coder, coder->right_dongle);
	if (!check_simulation_status(coder->env))
	{
		release_dongle(coder, coder->left_dongle);
		return (0);
	}
	return (1);
}

int	check_simulation_status(t_env *env)
{
	int	status;

	pthread_mutex_lock(&env->print_mutex);
	status = env->simulation_running;
	pthread_mutex_unlock(&env->print_mutex);
	return (status);
}
