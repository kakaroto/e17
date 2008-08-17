#ifndef _READER_H
#define _READER_H

/**
 *
 */
struct _Enesim_Component_Reader
{
	Enesim_Point 		*points;
	char 			*cmds;
	
	int 			pos;

	Enesim_Component 	*from; 	/**< Component the reader reads from */
	Enesim_Component 	*to; 	/**< In case the reader is used as a pipe */
	
};

void enesim_reader_notify(Enesim_Component_Reader *r);
void enesim_reader_reference_update(Enesim_Component_Reader *r);

#endif
