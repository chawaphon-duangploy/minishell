

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    +:+   */
/*                                                    +:+ +:+         +:+     */
/*   By: minishell                                  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/01 00:00:00 by minishell         #+#    #+#             */
/*   Updated: 2024/01/01 00:00:00 by minishell        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_status = 0;

static void	handle_main_sigint(int signum)
{
	(void)signum;
	g_status = SIGINT;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

static void	handle_child_sigint(int signum)
{
	(void)signum;
	g_status = SIGINT;
}

static void	handle_child_sigquit(int signum)
{
	(void)signum;
	g_status = SIGQUIT;
}

static void	set_signal(int signum, void (*handler)(int))
{
	struct sigaction	sa;

	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = handler;
	sigaction(signum, &sa, NULL);
}

void	signal_handler(t_sig_mode mode)
{
	g_status = 0;
	if (mode == MAIN)
	{
		set_signal(SIGINT, handle_main_sigint);
		set_signal(SIGQUIT, SIG_IGN);
	}
	else if (mode == CHILD)
	{
		set_signal(SIGINT, SIG_DFL);
		set_signal(SIGQUIT, SIG_DFL);
	}
	else if (mode == MAIN_CHILD)
	{
		set_signal(SIGINT, handle_child_sigint);
		set_signal(SIGQUIT, handle_child_sigquit);
	}
	else if (mode == HEREDOC)
	{
		set_signal(SIGINT, heredoc_interrupt);
		set_signal(SIGQUIT, SIG_IGN);
	}
	else if (mode == MAIN_HEREDOC)
	{
		set_signal(SIGINT, main_heredoc_interrupt);
		set_signal(SIGQUIT, SIG_IGN);
	}
}

void	print_signal_exit(int status)
{
	if (!WIFSIGNALED(status))
		return ;
	if (WTERMSIG(status) == SIGINT)
		write(STDOUT_FILENO, "\n", 1);
	else if (WTERMSIG(status) == SIGQUIT)
		ft_putendl_fd("Quit (core dumped)", STDERR_FILENO);
	else if (WTERMSIG(status) == SIGSEGV)
		ft_putendl_fd("Segmentation fault (core dumped)", STDERR_FILENO);
}