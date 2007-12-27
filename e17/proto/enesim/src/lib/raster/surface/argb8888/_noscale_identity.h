/**
 * parameters:
 * Enesim_Surface *ss
 * Enesim_Surface *ds
 */

/* TODO use srect and drect */
void 
ARGB8888_NOSCALE_IDENTITY_ITERATOR_PROTO
{
	DATA32 *dp;
	DATA32 *sp;
	int len;
	
	len = ds->h * ds->w;
	dp = ds->data.argb8888.data;
	sp = ss->data.argb8888.data;
	
	while (len--)
	{
		RENDER_OP_CALL
		dp++;
		sp++;
	}
}
