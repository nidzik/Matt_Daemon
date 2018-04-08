#include <stdio.h>
#include <string.h>
typedef struct s_p1
{
	int size;
	char type;
	char *string;
}				t_pkg1;

typedef struct s_p2
{
	char type;
	char pad[];
}				t_pkg2;

unsigned char * serialize_int(unsigned char *buffer, int value)
{
	/* Write big-endian int value into buffer; assumes 32-bit int and 8-bit char. */
	buffer[0] = value >> 24;
	buffer[1] = value >> 16;
	buffer[2] = value >> 8;
	buffer[3] = value;
	return buffer + 4;
}

unsigned char * serialize_char(unsigned char *buffer, char value)
{
	buffer[0] = value;
	return buffer + 1;
}
unsigned char * serialize_string(unsigned char *buffer, char *value)
{
	while (*value)
	{
		*buffer++ = *value++;
	}
	return (buffer + 1);
}
unsigned char * serialize_temp(unsigned char *buffer, t_pkg1 *tmp)
{
	buffer = serialize_int(buffer, tmp->size);
	buffer = serialize_char(buffer, tmp->type);
	buffer = serialize_string(buffer, tmp->string);
	return buffer;
}

void create_pkg1(char type, char *string, t_pkg1 *pkg)
{
	pkg->size = strlen(string)+ 1 + 3;
	pkg->type = type;
	pkg->string = string;

}

int main(void)
{
	struct s_p1 s1;
	struct s_p2 s2;

	int k = 0;
	unsigned char *ptr;
	unsigned char buf[1024] = {};
	char *mystr = "AAAABBBB123456789AAAA";
	ptr = NULL;
	s2.type = 'b';

	create_pkg1('A', mystr, &s1);

	printf("sizeof package 1 = %lu   pkg2 = %lu\n", sizeof(s1), sizeof(s2));

	ptr = serialize_temp(buf, &s1);

	for (k = 0; k < 100 ; k++)
		printf("%02X-",buf[k]);
	printf("\n");

	

	int a = (int)(buf[0]) << 24 |(buf[1]) << 16 | (buf[2]) << 8 | (buf[3]);
	printf("%d :) \n",a );

	unsigned char *str = buf +5;
	printf("%s :D \n",str );

	return 0;	
}
