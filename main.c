/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cduangpl <cduangpl@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/05 13:50:41 by cduangpl          #+#    #+#             */
/*   Updated: 2026/02/24 20:59:25 by cduangpl         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    +:+   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
** Temporary executor — runs a single builtin from argv.
** Replace this once executor.c / parser.c are built.
*/
static int	run_line(char **argv, char ***env_ptr)
{
	t_cmd_group	cmd;

	if (!argv || !argv[0])
		return (0);
	ft_bzero(&cmd, sizeof(t_cmd_group));
	cmd.cmd     = argv[0];
	cmd.argv    = argv;
	cmd.env_ptr = env_ptr;
	cmd.in_fd   = STDIN_FILENO;
	cmd.out_fd  = STDOUT_FILENO;
	if (is_builtin(argv[0]))
		return (execute_builtin(&cmd));
	ft_putstr_fd("minishell: command not found: ", STDERR_FILENO);
	ft_putendl_fd(argv[0], STDERR_FILENO);
	return (127);
}

/*
** Minimal tokenizer — splits on spaces only.
** Replace with lexer() once lexer.c is done.
*/
static char	**split_line(char *line)
{
	return (ft_split(line, ' '));
}

int	main(int argc, char **argv, char **envp)
{
	char	*line;
	char	**tokens;
	char	**env;
	char	***env_ptr;

	(void)argc;
	(void)argv;
	env = NULL;
	if (envp)
	{
		int i = 0;
		while (envp[i])
			i++;
		env = ft_calloc(i + 1, sizeof(char *));
		i = 0;
		while (envp[i])
		{
			env[i] = ft_strdup(envp[i]);
			i++;
		}
	}
	env_ptr = &env;
    signal_handler(MAIN);   // ← add this line
	while (1)
	{
		line = readline("minishell> ");
		if (line == NULL)
		{
			ft_putendl_fd("exit", STDOUT_FILENO);
			break ;
		}
		if (*line)
		{
			add_history(line);
			tokens = split_line(line);
			run_line(tokens, env_ptr);
			free_tab(tokens);
		}
		free(line);
	}
	return (0);
}
