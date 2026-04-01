#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ITERATIONS 1000000

typedef struct s_data {
	int             *counter;
    pthread_mutex_t mutex;
} t_data;

void *increment_routine(void *arg)
{
	t_data	*data;
	int		i;

	data = (t_data *)arg;
	i = 0;
	while (i < ITERATIONS)
	{
        
		(*(data->counter))++;
		i++;
        
	}
	return (NULL);
}

int main(void)
{
	pthread_t	th1;
	pthread_t	th2;
	int			counter;
	t_data		data;

	counter = 0;
	data.counter = &counter;
    pthread_mutex_init(&data.mutex, NULL);
	// Thread'leri oluştur
	if (pthread_create(&th1, NULL, &increment_routine, &data) != 0)
		return (1);
	if (pthread_create(&th2, NULL, &increment_routine, &data) != 0)
		return (2);
	// Thread'lerin bitmesini bekle
	if (pthread_join(th1, NULL) != 0)
		return (3);
	if (pthread_join(th2, NULL) != 0)
		return (4);
	printf("Final Counter: %d\n", counter);
	printf("Expected: %d\n", ITERATIONS * 2);
	return (0);
}
