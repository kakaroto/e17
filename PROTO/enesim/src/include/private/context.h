#ifndef CONTEXT_H_
#define CONTEXT_H_

struct _Enesim_Context
{
	Enesim_Rop rop;
	uint32_t color; /* ARGB representation of the color FIXME unpre or pre? */
	Enesim_Surface *src; /* Source Surface */
	Enesim_Surface *mask; /* Mask Surface */
	Eina_Rectangle clip;
	Enesim_Transformation *tx;
};


#endif /* CONTEXT_H_ */
