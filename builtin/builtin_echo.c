
#include "minishell.h"

static int	check_skip_n_flag(char *arg)
{
	if (ft_strncmp(arg, "-n", 2) == 0)
	{
		if (is_valid_echo_flag(arg + 2))
			return (1);
	}
	return (0);
}

static void	print_echo_args(t_cmd_group *cmd, int i, bool newline, int fd)
{
	while (cmd->argv[i] != NULL)
	{
		if (!newline && check_skip_n_flag(cmd->argv[i]))
		{
			i++;
			continue ;
		}
		ft_putstr_fd(cmd->argv[i], fd);
		if (cmd->argv[i + 1] != NULL)
			ft_putstr_fd(" ", fd);
		i++;
	}
}

int	builtin_echo(t_cmd_group *cmd)
{
	int		i;
	bool	newline;

	i = 1;
	newline = true;
	if (cmd->argv[1] && ft_strncmp(cmd->argv[1], "-n", 2) == 0)
	{
		if (is_valid_echo_flag(cmd->argv[1] + 2))
		{
			newline = false;
			i = 2;
		}
	}
	print_echo_args(cmd, i, newline, cmd->out_fd);
	if (newline)
		ft_putstr_fd("\n", cmd->out_fd);
	close_builtin_fds(cmd);
	return (0);
}