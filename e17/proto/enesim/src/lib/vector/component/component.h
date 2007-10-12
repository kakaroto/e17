#ifndef _COMPONENT_H
#define _COMPONENT_H

/**
 * @file
 * @brief Components
 * @defgroup Component_Internal_Group Component
 * @ingroup Enesim_Internal_Group
 * @{
 */

/**
 * To be documented
 * FIXME: To be fixed
 */
struct _Enesim_Component
{
	Enesim_Container		*path; 		/**< Main temporal data */
	Enesim_Component_Reader 	*src; 		/**< Source data */
	Edata_List		*readers;
	int 			has_changed; 	/**< Forward notification */
	int 			type;
	/* component specific data */
	const char 		*name; 		/**< Component's name */
	void 			*data; 		/**< Specific component data */
	void 			(*generate)(void *data, int *num);
	void 			(*free)(void *data);
};

/**
 * To be documented
 * FIXME: To be fixed
 */
enum
{
	ENESIM_COMPONENT_O, 	/**< Output Only Component */
	ENESIM_COMPONENT_IO,	/**< Input/Output Component */
	ENESIM_COMPONENT_TYPES
};

Enesim_Component * enesim_component_new(void);
void enesim_component_notify(Enesim_Component *c);
int enesim_component_generate(Enesim_Component *c, int *num);

/** @} */
#endif
