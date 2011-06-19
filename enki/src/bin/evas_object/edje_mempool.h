/*
 * edje_mempoll.h
 *
 *  Created on: Jun 19, 2011
 *      Author: watchwolf
 */

#ifndef EDJE_MEMPOOL_H_
#define EDJE_MEMPOOL_H_

#include "../main.h"

Evas_Object *
edje_mempool_object_add(const char *group);
void
edje_mempool_object_del(Evas_Object *obj);

#endif /* EDJE_MEMPOOL_H_ */
