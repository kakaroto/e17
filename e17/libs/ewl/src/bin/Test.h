#ifndef EWL_TEST_H
#define EWL_TEST_H

#include <Ewl.h>
#include "ewl-config.h"

#define HAVE___ATTRIBUTE__ 1

#if HAVE___ATTRIBUTE__
#define __UNUSED__ __attribute__((unused))
#else
#define __UNUSED__
#endif

enum Ewl_Test_Type
{
	EWL_TEST_TYPE_SIMPLE,
	EWL_TEST_TYPE_ADVANCED,
	EWL_TEST_TYPE_CONTAINER,
	EWL_TEST_TYPE_MISC
};
typedef enum Ewl_Test_Type Ewl_Test_Type;

typedef struct Ewl_Test Ewl_Test;
struct Ewl_Test
{
	void *handle;
	const char *name;
	int (*func)(Ewl_Container *con);
	const char *filename;
	const char *tip;
	Ewl_Test_Type type;
};

#endif

