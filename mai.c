#include "codexion.h"

int	main(int argc, char **argv)
{
	t_env	env;

	if (argc != 9)
		return (write(2, "Error: Wrong number of arguments\n", 33), 1);

	if (init_env(&env, argv) == FAILURE)
		return (1);

	if (start_simulation(&env) == FAILURE)
			return (cleanup(&env), 1);

	cleanup(&env);
	return (0);
}