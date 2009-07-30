/*
 * eon_sqpattern.h
 *
 *  Created on: 22-jul-2009
 *      Author: jl
 */

#ifndef EON_CHECKER_H_
#define EON_CHECKER_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_CHECKER_SW_CHANGED "swChanged"
#define EON_CHECKER_SH_CHANGED "shChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_CHECKER_SW;
extern Ekeko_Property_Id EON_CHECKER_SH;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Checker_Private Eon_Checker_Private;
struct _Eon_Checker
{
	Eon_Paint parent;
	Eon_Checker_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_checker_type_get(void);
EAPI Eon_Checker * eon_checker_new(void);

#endif /* EON_CHECKER_H_ */
