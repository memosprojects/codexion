#include "codexion.h"

int		start_simulation(t_env *env)
{
	pthread_t	monitor;
	int			i;

	i = 0;
	env->start_time = get_time_ms();

	while (i < env->num_coders)
	{
		if (pthread_create(&env->coders[i].thread, NULL, &coder_routine, &env->coders[i]) != 0)
			return (FAILURE);
		i++;
	}
	if (pthread_create(&monitor, NULL, &monitor_routine, env) != 0)
		return (FAILURE);
	i = 0;
	while (i < env->num_coders)
	{
		pthread_join(env->coders[i].thread, NULL);
		i++;
	}
	pthread_join(monitor, NULL);
	return (SUCCESS);
}

