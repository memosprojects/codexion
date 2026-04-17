#include "codexion.h"

// EDF için deadline hesapla (son derleme başlangıcı + burnout süresi)
long long get_deadline(t_coder *coder)
{
	long long deadline;

	pthread_mutex_lock(&coder->state_mutex);
	deadline = coder->last_compile_start + coder->env->time_to_burnout;
	pthread_mutex_unlock(&coder->state_mutex);
	return (deadline);
}

// Kuyruğa yeni bir node ekler (FIFO veya EDF'ye göre)
void add_to_queue(t_dongle *dongle, t_coder *coder)
{
	t_node *new_node;
	t_node **curr;

	new_node = malloc(sizeof(t_node));
	if (!new_node) return;
	new_node->coder_id = coder->id;
	new_node->deadline = get_deadline(coder);
	new_node->next = NULL;

	curr = (t_node **)&dongle->wait_queue;

	if (coder->env->scheduler == FIFO)
	{
		while (*curr)
			curr = &((*curr)->next);
		*curr = new_node;
	}
	else if (coder->env->scheduler == EDF)
	{
		while (*curr && (*curr)->deadline <= new_node->deadline)
			curr = &((*curr)->next);
		new_node->next = *curr;
		*curr = new_node;
	}
}

// Kuyruğun başındaki coder mı kontrol eder
int is_my_turn(t_dongle *dongle, t_coder *coder)
{
	t_node *head = (t_node *)dongle->wait_queue;
	if (head && head->coder_id == coder->id)
		return (1);
	return (0);
}

// İşlem bittiğinde coder'ı kuyruktan çıkarır
void remove_from_queue(t_dongle *dongle, int coder_id)
{
	t_node	**curr;
	t_node 	*temp;


	curr = (t_node **)&dongle->wait_queue;
	while (*curr)
	{
		if ((*curr)->coder_id == coder_id)
		{
			temp = *curr;
			*curr = (*curr)->next;
			free(temp);
			return ;
		}
		curr = &((*curr)->next);
	}
}
