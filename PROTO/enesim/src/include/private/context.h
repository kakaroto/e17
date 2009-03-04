#ifndef CONTEXT_H_
#define CONTEXT_H_

struct _Enesim_Context
{
	Enesim_Rop rop;
	uint32_t color; /* ARGB representation of the color FIXME unpre or pre? */
	Enesim_Surface *src; /* Source Surface */
	Enesim_Surface *mask; /* Mask Surface */
	struct
	{
		Eina_Bool used;
		Eina_Rectangle r;
	} clip;
	Enesim_Transformation *tx;
};


#endif /* CONTEXT_H_ */
