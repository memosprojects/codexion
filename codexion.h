#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/time.h>
# include <string.h>

/* Custom operation returns.*/
# define SUCCESS 0
# define FAILURE 1

/* Burnout control.*/
# define MONITOR_INTERVAL 1000

typedef enum e_sched {
	FIFO,
	EDF
}	t_sched;

/* ** Dongle struct:
** Holds mutex and priority queue.
*/
typedef struct s_dongle {
	pthread_mutex_t	mutex;
	long long		last_released_at;
	void			*wait_queue;
	pthread_cond_t	cond;
	int				owner_id; // FIFO/EDF heap yapısı için pointer
}	t_dongle;

typedef struct s_node {
    int             coder_id;
    long long       deadline; // Sadece EDF için gerekli
    struct s_node   *next;
} t_node;

/* ** Coder struct:
** Every thread holds its own info and env info. But not the other coders
*/
typedef struct s_coder {
	pthread_t		thread;
	int				id;
	int				compile_count;
	long long		last_compile_start;
	pthread_mutex_t state_mutex;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	struct s_env	*env;
}	t_coder;

typedef struct s_data {
	int             *counter;
    pthread_mutex_t mutex;
} t_data;

/* ** Env struct:
** Holds all args of the sim and situational sim data.
*/
typedef struct s_env {
	int				num_coders;
	int				time_to_burnout;
	int				time_to_compile;
	int				time_to_debug;
	int				time_to_refactor;
	int				num_compiles_req;
	int				cooldown;
    int             initialized_dongles;
    int             print_mutex_ok;
    int             state_mutex_ok;
	t_sched			scheduler;
	long long		start_time;
	int				simulation_running;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	state_mutex;
	t_coder			*coders;
	t_dongle		*dongles;
}	t_env;

/* Fonksiyon Prototipleri */

// Init & Cleanup
int			init_env(t_env *env, char **argv);
int			init_mutexes(t_env *env);
int         init_coder_info(t_env *env);
void		cleanup(t_env *env);

// Simulation Core
int			start_simulation(t_env *env);
void		*coder_routine(void *arg);
void		*monitor_routine(void *arg);
int			check_burnout(t_env *env);
int			burnout_flag(t_coder *coder, t_env *env);
int 		check_deployment(t_env *env);
int 		check_simulation_status(t_env *env);
void 		take_dongle(t_coder *coder, t_dongle *dongle);
void 		release_dongle(t_coder *coder, t_dongle *dongle);


// Utils & Timing
long long	get_time_ms(void);
void		precise_usleep(long long duration, t_env *env);
void		log_status(t_coder *coder, char *state);
void 		debug_coders(t_env *env);

// Queue Utils
long long get_deadline(t_coder *coder);
void add_to_queue(t_dongle *dongle, t_coder *coder);
int is_my_turn(t_dongle *dongle, t_coder *coder);
void remove_from_queue(t_dongle *dongle, int coder_id);
void clear_wait_queue(t_dongle *dongle);

#endif