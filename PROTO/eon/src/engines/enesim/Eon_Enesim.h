#ifndef _EON_ENESIM_H_
#define _EON_ENESIM_H_

#define EON_TYPE_ENGINE_ENESIM "Eon_Engine_Enesim"
typedef struct _Eon_Engine_Enesim_Private Eon_Engine_Enesim_Private;
typedef struct _Eon_Engine_Enesim
{
	Eon_Engine parent;
	Eon_Engine_Enesim_Private *prv;
} Eon_Engine_Enesim;

#endif
