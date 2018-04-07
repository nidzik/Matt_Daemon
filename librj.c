#include "rj.h"
#include "stdio.h"
char    *ft_strncpyz(char *dst, const char *src, size_t n)
{
    size_t i;

    i = 0;
    while (i < n)
    {
        dst[i] = src[i];
        i++;
    }
    while (i < n)
        dst[i++] = '\0';
    return (dst);
}
int		ft_strlen_16(char *str)
{
	int i ;
	int cmp;

	i = 0;
	cmp = 1;
	while (str[i] || i == 0)
	{
		i += 15;
		i++;
		if (str[i] == '\0')
		break ;
	}
	return i;
	
}
char        *ft_strncatz(char *dest, const char *src, size_t n)
{
    size_t  i;
    size_t  j;

    j = ft_strlen_16(dest);
    i = 0;
    while (i < n)
    {
        dest[j + i] = src[i];
        i++;
    }
    dest[j + i] = '\0';
    return (dest);
}
