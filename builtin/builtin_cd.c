
#include "minishell.h"

static int	update_pwd(char ***env_ptr, int update_old)
{
	char	cwd[PATH_MAX];
	char	*entry;

	if (!getcwd(cwd, sizeof(cwd)))
		return (0);
	entry = ft_strjoin(update_old ? "OLDPWD=" : "PWD=", cwd);
	if (!entry)
		return (0);
	set_env_var(env_ptr, entry);
	free(entry);
	return (1);
}

static void	revise_oldpwd(t_cmd_group *cmd, const char *oldpwd)
{
	char	*entry;

	entry = ft_strjoin("OLDPWD=", oldpwd);
	if (entry)
	{
		set_env_var(cmd->env_ptr, entry);
		free(entry);
	}
	perror("cd");
	close_builtin_fds(cmd);
}

static char	*expand_home(const char *path, const char *home)
{
	if (!path)
		return (NULL);
	if (path[0] != '~')
		return (ft_strdup(path));
	return (ft_strjoin(home, &path[1]));
}

static char	*resolve_path(const char *path, const char *oldpwd,
				const char *home, int fd)
{
	if (path[0] == '-' && path[1] == '\0')
	{
		ft_putendl_fd((char *)oldpwd, fd);
		return (ft_strdup(oldpwd));
	}
	if (path[0] == '~')
		return (expand_home(path, home));
	return (ft_strdup(path));
}

static int	update_oldpwd(char ***env_ptr, const char *saved_pwd)
{
	char	*old_entry;

	if (!saved_pwd[0])
		return (0);
	old_entry = ft_strjoin("OLDPWD=", saved_pwd);
	if (!old_entry)
		return (0);
	set_env_var(env_ptr, old_entry);
	free(old_entry);
	return (1);
}

static int	handle_cd(t_cmd_group *cmd, const char *path,
				const char *oldpwd, int fd)
{
	char	*home;
	char	*resolved;
	char	saved_pwd[PATH_MAX];

	home = ft_getenv(*(cmd->env_ptr), "HOME");
	if (!home && path[0] == '~')
	{
		ft_putstr_fd("cd: HOME not set\n", STDERR_FILENO);
		return (1);
	}
	if (!getcwd(saved_pwd, sizeof(saved_pwd)))
		saved_pwd[0] = '\0';
	resolved = resolve_path(path, oldpwd, home, fd);
	if (!resolved)
		return (1);
	if (chdir(resolved) != 0)
	{
		free(resolved);
		revise_oldpwd(cmd, oldpwd);
		return (1);
	}
	free(resolved);
	update_oldpwd(cmd->env_ptr, saved_pwd);
	update_pwd(cmd->env_ptr, 0);
	return (0);
}

int	builtin_cd(t_cmd_group *cmd)
{
	char	*path;
	char	*oldpwd;
	int		status;

	if (tab_len(cmd->argv) > 2)
		return (ft_putstr_fd("cd: too many arguments\n", STDERR_FILENO), 1);
	if (cmd->argv[1] == NULL)
	{
		path = ft_getenv(*(cmd->env_ptr), "HOME");
		if (path == NULL)
		{
			ft_putstr_fd("cd: HOME not set\n", STDERR_FILENO);
			close_builtin_fds(cmd);
			return (1);
		}
	}
	else
		path = cmd->argv[1];
	oldpwd = ft_getenv(*(cmd->env_ptr), "OLDPWD");
	status = handle_cd(cmd, path, oldpwd, cmd->out_fd);
	close_builtin_fds(cmd);
	return (status);
}
