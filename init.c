/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mehdemir <mehdemir@student.42berlin.d      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/20 13:23:45 by mehdemir          #+#    #+#             */
/*   Updated: 2026/04/20 13:23:48 by mehdemir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static int	is_valid_input(int argc, char **argv)
{
	int	i;
	int	j;

	i = 1;
	while (i < argc - 1)
	{
		j = 0;
		if (argv[i][j] == '\0')
			return (FAILURE);
		while (argv[i][j])
		{
			if (argv[i][j] < '0' || argv[i][j] > '9')
				return (FAILURE);
			j++;
		}
		i++;
	}
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
		return (FAILURE);
	return (SUCCESS);
}

int	init_env(t_env *env, char **argv)
{
	if (!env)
		return (FAILURE);
	if (is_valid_input(9, argv) == FAILURE)
		return (write(2, "Error: Invalid arguments\n", 25), FAILURE);
	env->num_coders = atoi(argv[1]);
	env->time_to_burnout = atoi(argv[2]);
	env->time_to_compile = atoi(argv[3]);
	env->time_to_debug = atoi(argv[4]);
	env->time_to_refactor = atoi(argv[5]);
	env->num_compiles_req = atoi(argv[6]);
	env->cooldown = atoi(argv[7]);
	if (strcmp(argv[8], "fifo") == 0)
		env->scheduler = FIFO;
	else
		env->scheduler = EDF;
	env->simulation_running = 1;
	env->start_time = get_time_ms();
	env->coders = malloc(sizeof(t_coder) * env->num_coders);
	env->dongles = malloc(sizeof(t_dongle) * env->num_coders);
	if (!env->coders || !env->dongles)
		return (cleanup(env), FAILURE);
	if (init_mutexes(env) == FAILURE)
		return (cleanup(env), FAILURE);
	init_coder_info(env);
	return (SUCCESS);
}

int	init_mutexes(t_env *env)
{
	env->initialized_dongles = 0;
	while (env->initialized_dongles < env->num_coders)
	{
		if (
			pthread_mutex_init(
				&env->dongles[env->initialized_dongles].mutex, NULL) != 0)
			return (FAILURE);
		if (
			pthread_cond_init(
				&env->dongles[env->initialized_dongles].cond, NULL) != 0)
			return (FAILURE);
		env->dongles[env->initialized_dongles].wait_queue = NULL;
		env->dongles[env->initialized_dongles].owner_id = 0;
		env->dongles[env->initialized_dongles].last_released_at = 0;
		env->initialized_dongles++;
	}
	if (pthread_mutex_init(&env->print_mutex, NULL) != 0)
		return (FAILURE);
	env->print_mutex_ok = 1;
	if (pthread_mutex_init(&env->state_mutex, NULL) != 0)
		return (FAILURE);
	env->state_mutex_ok = 1;
	return (SUCCESS);
}

int	init_coder_info(t_env *env)
{
	int	i;

	i = 0;
	while (i < env->num_coders)
	{
		env->coders[i].id = i + 1;
		env->coders[i].compile_count = 0;
		env->coders[i].left_dongle = &env->dongles[i];
		env->coders[i].right_dongle = &env->dongles[(i + 1) % env->num_coders];
		env->coders[i].env = env;
		env->coders[i].last_compile_start = env->start_time;
		env->dongles[i].last_released_at = 0;
		pthread_mutex_init(&env->coders[i].state_mutex, NULL);
		i++;
	}
	return (0);
}

void	cleanup(t_env *env)
{
	int	i;

	if (!env)
		return ;
	if (env->print_mutex_ok)
		pthread_mutex_destroy(&env->print_mutex);
	if (env->state_mutex_ok)
		pthread_mutex_destroy(&env->state_mutex);
	i = 0;
	while (i < env->initialized_dongles)
	{
		clear_wait_queue(&env->dongles[i]);
		pthread_mutex_destroy(&env->dongles[i].mutex);
		pthread_cond_destroy(&env->dongles[i].cond);
		i++;
	}
	if (env->dongles)
		free(env->dongles);
	if (env->coders)
		free(env->coders);
}
