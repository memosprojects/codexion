#include "codexion.h"

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (coder->id % 2 == 0)
		precise_usleep(1, coder->env);
	while (check_simulation_status(coder->env))
	{
		//refactor
		log_status(coder, "is refactoring");
		precise_usleep(coder->env->time_to_refactor, coder->env);
		//takedongle
		take_dongle(coder, coder->left_dongle);
		if (!check_simulation_status(coder->env))
			break; // Burnout kontrolü
		take_dongle(coder, coder->right_dongle);
		if (!check_simulation_status(coder->env))
		{
			release_dongle(coder, coder->left_dongle); // İkinciyi alamazsa birinciyi bırak
			break;
		}
		//compile
		pthread_mutex_lock(&coder->state_mutex);
		coder->last_compile_start = get_time_ms();
		pthread_mutex_unlock(&coder->state_mutex);
		log_status(coder, "is compiling");
		precise_usleep(coder->env->time_to_compile, coder->env);
		pthread_mutex_lock(&coder->state_mutex);
		coder->compile_count++;
		pthread_mutex_unlock(&coder->state_mutex);
		//releasedongle
		release_dongle(coder, coder->left_dongle);
		release_dongle(coder, coder->right_dongle);
		//debug
		log_status(coder, "is debugging");
		precise_usleep(coder->env->time_to_debug, coder->env);
	}
	return (NULL);

}

void take_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	
	// Kuyruğa gir
	add_to_queue(dongle, coder);

	// Sıra bende değilse veya dongle birindeyse uyu
	while (check_simulation_status(coder->env) && 
		(dongle->owner_id != 0 || !is_my_turn(dongle, coder)))
	{
		pthread_cond_wait(&dongle->cond, &dongle->mutex);
	}

	// Dongle'ı al
	if (check_simulation_status(coder->env))
	{
		dongle->owner_id = coder->id;
		log_status(coder, "has taken a dongle");
	}

	pthread_mutex_unlock(&dongle->mutex);
}

void release_dongle(t_coder *coder, t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	
	dongle->owner_id = 0;
	dongle->last_released_at = get_time_ms();
	
	// Kuyruktan çık ve diğerlerini uyandır
	remove_from_queue(dongle, coder->id);
	pthread_cond_broadcast(&dongle->cond);
	
	pthread_mutex_unlock(&dongle->mutex);
}

int check_simulation_status(t_env *env)
{
	int status;

	pthread_mutex_lock(&env->print_mutex); // Veya ayrı bir status_mutex
	status = env->simulation_running;
	pthread_mutex_unlock(&env->print_mutex);
	return (status);
}