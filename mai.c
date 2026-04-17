#include "codexion.h"

int	main(int argc, char **argv)
{
	t_env	env;
	char	*err;

	if (argc != 9)
	{
		err = "Error: Wrong number of arguments\n";
		return (write(2, err, strlen(err)), 1);
	}

	if (init_env(&env, argv) == FAILURE)
		return (1);

	if (start_simulation(&env) == FAILURE)
		return (cleanup(&env), 1);

	cleanup(&env);
	return (0);
}