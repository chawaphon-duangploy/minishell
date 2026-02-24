
#include "minishell.h"

int	builtin_unset(t_cmd_group *cmd, char ***env_ptr)
{
	int	i;

	i = 1;
	while (cmd->argv[i])
	{
		inner_unset(cmd->argv[i], env_ptr);
		i++;
	}
	close_builtin_fds(cmd);
	return (0);
}