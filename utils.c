
// utils.c
#include "minishell.h"

void    close_fd(int fd)
{
    if (fd > 2)
        close(fd);
}

int     tab_len(char **tab)
{
    int i;

    i = 0;
    if (!tab)
        return (0);
    while (tab[i])
        i++;
    return (i);
}

char    *ft_getenv(char **env, const char *key)
{
    int i;
    int len;

    if (!env || !key)
        return (NULL);
    len = ft_strlen(key);
    i = 0;
    while (env[i])
    {
        if (ft_strncmp(env[i], key, len) == 0 && env[i][len] == '=')
            return (&env[i][len + 1]);
        i++;
    }
    return (NULL);
}

char    **copy_tab(char **tab)
{
    char    **copy;
    int     len;
    int     i;

    len = tab_len(tab);
    copy = ft_calloc(len + 1, sizeof(char *));
    if (!copy)
        return (NULL);
    i = 0;
    while (i < len)
    {
        copy[i] = ft_strdup(tab[i]);
        if (!copy[i])
        {
            while (--i >= 0)
                free(copy[i]);
            free(copy);
            return (NULL);
        }
        i++;
    }
    copy[len] = NULL;
    return (copy);
}

void    *ft_safe_calloc(size_t count, size_t size, char *msg)
{
    void    *ptr;

    ptr = ft_calloc(count, size);
    if (!ptr)
        exit_msg(msg);
    return (ptr);
}

int     ft_strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return ((unsigned char)*s1 - (unsigned char)*s2);
}

void    free_tab(char **tab)
{
    int i;

    i = 0;
    if (!tab)
        return ;
    while (tab[i])
        free(tab[i++]);
    free(tab);
}

//utils token
int     is_whitespace(char c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

int     is_quotation_mark(char c)
{
    return (c == '\'' || c == '"');
}

int     is_special_char(char c)
{
    return (is_whitespace(c) || c == '|' || c == '<' || c == '>'
        || c == '(' || c == ')' || c == '&');
}