static inline FIXED_TYPE FIXED_INT_FROM(int v)
{
	return v << FIXED_FLOAT_BITS;
}

static inline FIXED_TYPE FIXED_INT_TO(FIXED_TYPE v)
{
	return v >> FIXED_FLOAT_BITS;
}
	
static inline FIXED_TYPE FIXED_FLOAT_FROM(float v)
{
	FIXED_TYPE r;
	r = (FIXED_TYPE)(v * FIXED_DIV + (v < 0 ? -0.5 : 0.5));
	return r;
}

static inline float FIXED_FLOAT_TO(FIXED_TYPE v)
{
	float r;
	r = v / FIXED_DIV;
	return r;
}

static inline FIXED_TYPE FIXED_ADD(FIXED_TYPE a, FIXED_TYPE b)
{
	return a + b;
}

static inline FIXED_TYPE FIXED_SUB(a, b)
{
	return a - b;
}

static inline FIXED_TYPE FIXED_MUL(a, b)
{
	return (a * b) >> FIXED_FLOAT_BITS;
}

static inline FIXED_TYPE FIXED_SQRT(FIXED_TYPE a)
{
	unsigned int root, remHi, remLo, testDiv, count;
	root = 0; /* Clear root */
	remHi = 0; /* Clear high part of partial remainder */
	remLo = a; /* Get argument into low part of partial remainder */
	count = FIXED_ITERS; /* Load loop counter */
	do
	{
		remHi = (remHi<<2) | (remLo>>30);
		remLo <<= 2; /* get 2 bits of arg */
		root <<= 1; /* Get ready for the next bit in the root */
		testDiv = (root << 1) + 1; /* Test radical */
		if (remHi >= testDiv)
		{
			remHi -= testDiv;
			root++;
		}
	} while (count-- != 0);
	return (root);	
}

static inline unsigned int FIXED_FRACC_GET(FIXED_TYPE v)
{
	return (v & 0xffff);
}
