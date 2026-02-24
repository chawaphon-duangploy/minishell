
#include "minishell.h"

static int	is_token(char *str)
{
	if (str == NULL)
		return (0);
	if (ft_strncmp("&&", str, 2) == 0)
		return (2);
	if (ft_strncmp("||", str, 2) == 0)
		return (2);
	if (ft_strncmp("<<", str, 2) == 0)
		return (2);
	if (ft_strncmp(">>", str, 2) == 0)
		return (2);
	if (str[0] == '&' || str[0] == '|')
		return (1);
	if (str[0] == '(' || str[0] == ')')
		return (1);
	if (str[0] == '<' || str[0] == '>')
		return (1);
	return (0);
}

static int	token_len(char *str)
{
	char	open;
	int		len;

	len = is_token(str);
	if (len != 0)
		return (len);
	len = 0;
	while (str[len] != '\0' && !is_special_char(str[len]))
	{
		if (is_quotation_mark(str[len]))
		{
			open = str[len];
			len++;
			while (str[len] != '\0' && str[len] != open)
				len++;
			if (str[len] == open)
				len++;
		}
		else
			len++;
	}
	return (len);
}

static int	count_words(char *str)
{
	int	i;
	int	cnt;

	i = 0;
	cnt = 0;
	while (str[i] != '\0')
	{
		while (str[i] != '\0' && is_whitespace(str[i]))
			i++;
		if (str[i] == '\0')
			break ;
		cnt++;
		i += token_len(&str[i]);
	}
	return (cnt);
}

static void	fill_str(char *dst, char *src, int len)
{
	int	i;

	i = 0;
	while (i < len)
	{
		dst[i] = src[i];
		i++;
	}
	dst[len] = '\0';
}

char	**tokenizer(char *str)
{
	int		i;
	int		j;
	int		size;
	int		len;
	char	**tab;

	size = count_words(str);
	tab = ft_safe_calloc(size + 1, sizeof(char *), "");
	if (tab == NULL)
		return (NULL);
	tab[size] = NULL;
	i = -1;
	j = 0;
	while (++i < size)
	{
		while (str[j] != '\0' && is_whitespace(str[j]))
			j++;
		len = token_len(&str[j]);
		tab[i] = ft_safe_calloc(len + 1, sizeof(char), "");
		if (tab[i] == NULL)
			return (NULL);
		fill_str(tab[i], &str[j], len);
		j += len;
	}
	return (tab);
}
