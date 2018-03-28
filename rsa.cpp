#include <rsa.h>

struct RSA
{
    typedef RSAFunction PublicKey;
    typedef InvertibleRSAFunction PrivateKey;
};

int main(int ac, char **av)
{
	InvertibleRSAFunction params;

	params.GenerateRandomWithKeySize(rng, 3072);
	return 0;
}
