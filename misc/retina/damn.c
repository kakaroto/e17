#include <stdio.h>
#include <math.h>

int
main(int argc, char **argv)
{
	char k[10];
	char c = 'a', d;
	int a;

	sprintf(k, "%s", argv[1]);

	/* actual alg */
	a = c + (int)k[1] * (int)k[0] + (int)k[2] - (int)k[4] * (int)k[3];
	a = a * (int)k[5] * (int)k[7] + (int)k[
	
	d = (char)a;
	
	printf("%c ciphered = (%d) %c\n", c, a, d);

	return 0;
}
