#include <math.h>

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

//#define P 0x7FFFFFFFFFFFFFFF
/*
// Power function to return value of a ^ b mod P
long long int power(long long int a, long long int b,\
					long long int P)
{
    if (b == 1)
        return a;

    else
        return (((long long int)pow(a, b)) % P);
		}*/
// calc a * b % p , avoid 64bit overflow
static inline uint64_t
mul_mod_p(uint64_t a, uint64_t b, uint64_t P)
{
    uint64_t m = 0;
    uint64_t t = 0;

    while (b) 
	{
        if (b & 1) 
		{
            t = P-a;
            if ( m >= t) 
                m -= t;
             else
                m += a;
        }
        if (a >= P - a) 
            a = a * 2 - P;
		else
            a = a * 2;
        b>>=1;
    }
    return m;
}

static inline uint64_t
pow_mod_p(uint64_t a, uint64_t b, uint64_t P)
{
    uint64_t t = 0;
	
	if (b == 1) 
        return a;
	
	t = pow_mod_p(a, b >> 1, P);
    t = mul_mod_p(t, t, P);
    if (b % 2)
        t = mul_mod_p(t, a, P);
    return t;
}

// calc a^b % p
uint64_t
powmodp(uint64_t a, uint64_t b, uint64_t P) 
{
    if (a > P)
        a %= P;
    return pow_mod_p(a, b, P);
}

uint64_t rand_uint64(void) {
    uint64_t r = 0;
    for (int i=0; i<64; i += 30) {
        r = r*((uint64_t)RAND_MAX + 1) + rand();
    }
    return r;
}
uint64_t rand_uint64_slow(void) {
    uint64_t r = 0;
    for (int i=0; i<64; i++) {
        r = r*2 + rand()%2;
    }
    return r;
}
int is_prime(int num)
{
    if (num <= 1) return 0;
    if (num % 2 == 0 && num > 2) return 0;
    for(int i = 3; i < num / 2; i+= 2)
    {
        if (num % i == 0 )
            return 0;
    }
    return 1;
}

#include <stdlib.h>
#include <stdio.h>

int main (int ac, char **av)
{
	unsigned long long int  P, G, x, a, y, ka, kb, j;

	unsigned long long  b;
	j = 0;
	int n, i = 3, count, c;
	P = 0;
	n = 9706108;
    srand(time(NULL));
/*	for ( count = 2 ; count <= n ;  )
	{
		for ( c = 2626570 ; c <= i - 1 ; c++ )
		{
			if ( i%c == 0 )
				break;
		}
		if ( c == i )
		{
			printf("%d\n", i);
*/

			// Both the persons will be agreed upon the
			// public keys G and P
	// A prime number P is taken
	while (!is_prime(rand_uint64()))
		P  = rand_uint64();
		;

			printf("The value of P : %lld\n", P);
//			while (G < P/2)
			{
				G = 5;//i % P;//9706108; // A primitve root for P, G is taken
				printf("The value of G : %lld\n\n", G);

				// Alice will choose the private key a
				a = 77100074583237325; // a is the chosen private key
				printf("The private key a for Alice : %lld\n", a);
				x = powmodp(G, a, P); // gets the generated key
				printf("The public shared key x (Alice -> Bob) : %lld\n\n", x);

				// Bob will choose the private key b
				b = 91958578577973254; // b is the chosen private key
				printf("The private key b for Bob : %lld\n", b);
				y = powmodp(G, b, P); // gets the generated key
				printf("The public shared key y (Bob -> Alice) : %lld\n\n", y);

				// Generating the secret key after the exchange
				// of keys
				ka = powmodp(y, a, P); // Secret key for Alice
				kb = powmodp(x, b, P); // Secret key for Bob

				if (kb == ka && ka > 0 && G != 1)
					fprintf(stderr, "G = %llu   P = %llu\n",G, P);
//	x = 0; y = 0;
				j++;
/*			}
	j = 0;
	count++;
		}
		i++;*/
	}

    printf("Secret key for the Alice is : %llu (shared bob key ^ priv alice key  mod Prime)\n", ka)\
		;
    printf("Secret Key for the Bob is : %llu\n", kb);
	return 0;
}

/*
https://tools.ietf.org/html/rfc2409#section-6.2

https://crypto.stackexchange.com/questions/820/how-does-one-calculate-a-primitive-root-for-diffie-hellman
*/
