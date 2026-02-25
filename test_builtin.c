/* ************************************************************************** */
/*                                                                            */
/*   test_builtin.c                                                           */
/*   Compile: make test                                                       */
/*   Run:     ./test_builtin                                                  */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* ── colour codes ────────────────────────────────────────────────────────── */
#define GRN "\033[0;32m"
#define RED "\033[0;31m"
#define YEL "\033[0;33m"
#define CYN "\033[0;36m"
#define RST "\033[0m"

/* ── global required by signals.c / builtins ─────────────────────────────── */

/* ── test counter ────────────────────────────────────────────────────────── */
static int	g_pass = 0;
static int	g_fail = 0;

static void	result(const char *name, int ok)
{
	if (ok)
	{
		printf(GRN "[PASS]" RST " %s\n", name);
		g_pass++;
	}
	else
	{
		printf(RED "[FAIL]" RST " %s\n", name);
		g_fail++;
	}
}

/* ── helper: build a t_cmd_group on the stack ────────────────────────────── */
static t_cmd_group	make_cmd(char **argv, char ***env_ptr)
{
	t_cmd_group	cmd;

	ft_bzero(&cmd, sizeof(t_cmd_group));
	cmd.argv    = argv;
	cmd.cmd     = argv[0];
	cmd.env_ptr = env_ptr;
	cmd.in_fd   = STDIN_FILENO;
	cmd.out_fd  = STDOUT_FILENO;
	return (cmd);
}

/* ── helper: deep-copy environ into heap char** ──────────────────────────── */
static char	**copy_environ(char **envp)
{
	int		len;
	char	**env;
	int		i;

	len = 0;
	while (envp[len])
		len++;
	env = ft_calloc(len + 1, sizeof(char *));
	if (!env)
		exit(1);
	i = 0;
	while (i < len)
	{
		env[i] = ft_strdup(envp[i]);
		i++;
	}
	env[len] = NULL;
	return (env);
}

/* ══════════════════════════════════════════════════════════════════════════ */
/*  SECTION 1 — is_builtin dispatcher                                        */
/* ══════════════════════════════════════════════════════════════════════════ */

static void	test_is_builtin(void)
{
	printf(CYN "\n── is_builtin ──\n" RST);
	result("echo  is builtin", is_builtin("echo")   == 1);
	result("cd    is builtin", is_builtin("cd")     == 1);
	result("pwd   is builtin", is_builtin("pwd")    == 1);
	result("env   is builtin", is_builtin("env")    == 1);
	result("exit  is builtin", is_builtin("exit")   == 1);
	result("export is builtin", is_builtin("export") == 1);
	result("unset is builtin", is_builtin("unset")  == 1);
	result("ls    NOT builtin", is_builtin("ls")    == 0);
	result("cat   NOT builtin", is_builtin("cat")   == 0);
	result("NULL  NOT builtin", is_builtin(NULL)    == 0);
}

/* ══════════════════════════════════════════════════════════════════════════ */
/*  SECTION 2 — builtin_echo                                                 */
/* ══════════════════════════════════════════════════════════════════════════ */

static void	test_echo(char ***env_ptr)
{
	t_cmd_group	cmd;

	printf(CYN "\n── builtin_echo ──\n" RST);

	/* basic echo */
	printf("  expect → [hello world]\n  got    → [");
	fflush(stdout);
	char *a1[] = {"echo", "hello", "world", NULL};
	cmd = make_cmd(a1, env_ptr);
	result("echo hello world rc=0", builtin_echo(&cmd) == 0);

	/* -n flag: no newline */
	printf("  expect → [no newline here]  (no \\n)\n  got    → [");
	fflush(stdout);
	char *a2[] = {"echo", "-n", "no newline here", NULL};
	cmd = make_cmd(a2, env_ptr);
	result("echo -n rc=0", builtin_echo(&cmd) == 0);
	printf("\n");

	/* empty echo */
	printf("  expect → [empty line]\n  got    → [");
	fflush(stdout);
	char *a3[] = {"echo", NULL};
	cmd = make_cmd(a3, env_ptr);
	result("echo (no args) rc=0", builtin_echo(&cmd) == 0);

	/* multiple -n */
	printf("  expect → [abc] no newline\n  got    → [");
	fflush(stdout);
	char *a4[] = {"echo", "-n", "-n", "abc", NULL};
	cmd = make_cmd(a4, env_ptr);
	result("echo -n -n abc rc=0", builtin_echo(&cmd) == 0);
	printf("\n");
}

/* ══════════════════════════════════════════════════════════════════════════ */
/*  SECTION 3 — builtin_pwd                                                  */
/* ══════════════════════════════════════════════════════════════════════════ */

static void	test_pwd(char ***env_ptr)
{
	t_cmd_group	cmd;
	char		cwd_before[PATH_MAX];

	printf(CYN "\n── builtin_pwd ──\n" RST);
	getcwd(cwd_before, sizeof(cwd_before));

	char *a1[] = {"pwd", NULL};
	cmd = make_cmd(a1, env_ptr);
	printf("  expect → [%s]\n  got    → [", cwd_before);
	fflush(stdout);
	result("pwd rc=0", builtin_pwd(&cmd) == 0);
}

/* ══════════════════════════════════════════════════════════════════════════ */
/*  SECTION 4 — builtin_cd                                                   */
/* ══════════════════════════════════════════════════════════════════════════ */

static void	test_cd(char ***env_ptr)
{
	t_cmd_group	cmd;
	char		cwd[PATH_MAX];

	printf(CYN "\n── builtin_cd ──\n" RST);

	/* cd /tmp */
	char *a1[] = {"cd", "/tmp", NULL};
	cmd = make_cmd(a1, env_ptr);
	int rc1 = builtin_cd(&cmd);
	getcwd(cwd, sizeof(cwd));
	result("cd /tmp rc=0",       rc1 == 0);
	result("cd /tmp cwd==/tmp",  ft_strncmp(cwd, "/tmp", 4) == 0);

	/* cd back somewhere valid */
	char *a2[] = {"cd", "/", NULL};
	cmd = make_cmd(a2, env_ptr);
	result("cd / rc=0", builtin_cd(&cmd) == 0);
	getcwd(cwd, sizeof(cwd));
	result("cd / cwd==/", ft_strncmp(cwd, "/", 2) == 0);

	/* cd to nonexistent path → must return non-zero */
	char *a3[] = {"cd", "/this_path_does_not_exist_42", NULL};
	cmd = make_cmd(a3, env_ptr);
	result("cd bad path rc!=0", builtin_cd(&cmd) != 0);

	/* cd with no args → should go to HOME */
	char *home = get_env_value(*env_ptr, "HOME");
	if (home)
	{
		char *a4[] = {"cd", NULL};
		cmd = make_cmd(a4, env_ptr);
		int rc4 = builtin_cd(&cmd);
		getcwd(cwd, sizeof(cwd));
		result("cd (no arg) goes HOME rc=0", rc4 == 0);
		result("cd (no arg) cwd==HOME", ft_strncmp(cwd, home, ft_strlen(home)) == 0);
	}
	else
		printf(YEL "  [SKIP] HOME not set\n" RST);
}

/* ══════════════════════════════════════════════════════════════════════════ */
/*  SECTION 5 — builtin_env                                                  */
/* ══════════════════════════════════════════════════════════════════════════ */

static void	test_env(char ***env_ptr)
{
	t_cmd_group	cmd;

	printf(CYN "\n── builtin_env ──\n" RST);

	char *a1[] = {"env", NULL};
	cmd = make_cmd(a1, env_ptr);
	printf("  (env output printed below — check it looks like KEY=value)\n");
	int rc = builtin_env(&cmd);
	result("env rc=0", rc == 0);
}

/* ══════════════════════════════════════════════════════════════════════════ */
/*  SECTION 6 — builtin_export & builtin_unset                               */
/* ══════════════════════════════════════════════════════════════════════════ */

static void	test_export_unset(char ***env_ptr)
{
	t_cmd_group	cmd;

	printf(CYN "\n── builtin_export ──\n" RST);

	/* export NEW_VAR=hello42 */
	char *a1[] = {"export", "NEW_VAR=hello42", NULL};
	cmd = make_cmd(a1, env_ptr);
	result("export NEW_VAR=hello42 rc=0", builtin_export(&cmd, env_ptr) == 0);
	result("NEW_VAR visible via get_env_value",
		ft_strncmp(get_env_value(*env_ptr, "NEW_VAR"), "hello42", 7) == 0);

	/* export overwrite */
	char *a2[] = {"export", "NEW_VAR=changed", NULL};
	cmd = make_cmd(a2, env_ptr);
	builtin_export(&cmd, env_ptr);
	result("export overwrites existing key",
		ft_strncmp(get_env_value(*env_ptr, "NEW_VAR"), "changed", 7) == 0);

	/* export invalid identifier */
	char *a3[] = {"export", "1BADNAME=val", NULL};
	cmd = make_cmd(a3, env_ptr);
	result("export invalid id rc!=0", builtin_export(&cmd, env_ptr) != 0);

	/* export with no args — should print exported vars, return 0 */
	char *a4[] = {"export", NULL};
	cmd = make_cmd(a4, env_ptr);
	printf("  (export no-args output — should list declare -x lines)\n");
	result("export no-args rc=0", builtin_export(&cmd, env_ptr) == 0);

	printf(CYN "\n── builtin_unset ──\n" RST);

	/* unset NEW_VAR */
	char *a5[] = {"unset", "NEW_VAR", NULL};
	cmd = make_cmd(a5, env_ptr);
	result("unset NEW_VAR rc=0", builtin_unset(&cmd, env_ptr) == 0);
	result("NEW_VAR gone after unset",
		get_env_value(*env_ptr, "NEW_VAR") == NULL);

	/* unset non-existent key — bash returns 0 */
	char *a6[] = {"unset", "DOES_NOT_EXIST_XYZ", NULL};
	cmd = make_cmd(a6, env_ptr);
	result("unset non-existent rc=0", builtin_unset(&cmd, env_ptr) == 0);

	/* unset PATH — careful, verify it's gone */
	char *a7[] = {"unset", "MYTEST_PATH", NULL};
	/* first set it */
	char *sa[] = {"export", "MYTEST_PATH=/usr/bin", NULL};
	cmd = make_cmd(sa, env_ptr);
	builtin_export(&cmd, env_ptr);
	cmd = make_cmd(a7, env_ptr);
	builtin_unset(&cmd, env_ptr);
	result("unset MYTEST_PATH gone",
		get_env_value(*env_ptr, "MYTEST_PATH") == NULL);
}

/* ══════════════════════════════════════════════════════════════════════════ */
/*  SECTION 7 — builtin_utils helpers                                        */
/* ══════════════════════════════════════════════════════════════════════════ */

static void	test_utils(char ***env_ptr)
{
	printf(CYN "\n── builtin_utils ──\n" RST);

	/* is_valid_identifier */
	result("valid id: FOO",       is_valid_identifier("FOO")      == true);
	result("valid id: _foo123",   is_valid_identifier("_foo123")  == true);
	result("invalid id: 1abc",    is_valid_identifier("1abc")     == false);
	result("invalid id: fo-o",    is_valid_identifier("fo-o")     == false);
	result("invalid id: (empty)", is_valid_identifier("")         == false);

	/* find_env_index */
	char **env = *env_ptr;
	result("find PATH index >= 0", find_env_index(env, "PATH") >= 0);
	result("find FAKE_XYZ = -1",   find_env_index(env, "FAKE_XYZ_NOTEXIST") == -1);

	/* is_valid_echo_flag */
	result("valid echo flag -n",   is_valid_echo_flag("-n")  == true);
	result("invalid echo flag -x", is_valid_echo_flag("-x")  == false);
	result("invalid echo flag abc",is_valid_echo_flag("abc") == false);

	/* is_valid_numeric */
	result("valid numeric 0",      is_valid_numeric("0")    == true);
	result("valid numeric 42",     is_valid_numeric("42")   == true);
	result("valid numeric -1",     is_valid_numeric("-1")   == true);
	result("invalid numeric abc",  is_valid_numeric("abc")  == false);
	result("invalid numeric 1a",   is_valid_numeric("1a")   == false);
}

/* ══════════════════════════════════════════════════════════════════════════ */
/*  SUMMARY                                                                  */
/* ══════════════════════════════════════════════════════════════════════════ */

static void	print_summary(void)
{
	int total = g_pass + g_fail;
	printf(CYN "\n══════════════════════════════\n" RST);
	printf("  Results: %d / %d tests passed\n", g_pass, total);
	if (g_fail == 0)
		printf(GRN "  All tests passed! ✓\n" RST);
	else
		printf(RED "  %d test(s) failed.\n" RST, g_fail);
	printf(CYN "══════════════════════════════\n\n" RST);
}

/* ══════════════════════════════════════════════════════════════════════════ */
/*  main                                                                     */
/* ══════════════════════════════════════════════════════════════════════════ */

int	main(int argc, char **argv, char **envp)
{
	char	**env;
	char	***env_ptr;

	(void)argc;
	(void)argv;

	/* initialise env from the real process environment */
	env = copy_environ(envp);
	env_ptr = &env;

	printf(CYN "══════════════════════════════\n");
	printf(" minishell builtin test suite\n");
	printf("══════════════════════════════\n" RST);

	test_is_builtin();
	test_echo(env_ptr);
	test_pwd(env_ptr);
	test_cd(env_ptr);
	test_env(env_ptr);
	test_export_unset(env_ptr);
	test_utils(env_ptr);

	print_summary();

	/* free env */
	int i = 0;
	while (env[i])
		free(env[i++]);
	free(env);

	return (g_fail > 0 ? 1 : 0);
}