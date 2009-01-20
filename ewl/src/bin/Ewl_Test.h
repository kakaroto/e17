/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_TEST_H
#define EWL_TEST_H

#include "ewl_base.h"

/**
 * @enum Ewl_Test_Type
 * The different types of tests available
 */
enum Ewl_Test_Type
{
        EWL_TEST_TYPE_SIMPLE,
        EWL_TEST_TYPE_ADVANCED,
        EWL_TEST_TYPE_CONTAINER,
        EWL_TEST_TYPE_MISC,
        EWL_TEST_TYPE_UNIT
};
typedef enum Ewl_Test_Type Ewl_Test_Type;

/**
 * Provides information on a unit test
 */
typedef struct Ewl_Unit_Test Ewl_Unit_Test;

/**
 * @brief Contains the information needed to run a unit test
 */
struct Ewl_Unit_Test
{
        const char *name;               /**< The test name */
        int (*func)(char *buf, int len);/**< The test function */

        char *failure_reason;           /**< Test failure reason */
        short status;                   /**< Test status */
        unsigned int quiet: 1;          /**< Boolean to disable stderr */
};

/**
 * Provides information on an Ewl Test set
 */
typedef struct Ewl_Test Ewl_Test;

/**
 * @brief Holds information on an Ewl Test set
 */
struct Ewl_Test
{
        const char *name;               /**< The test name */
        const char *filename;           /**< Filename of the test */
        const char *tip;                /**< Tooltip for the test */

        Ecore_Plugin *plugin;           /**< Dynamic file handle for test */
        Ewl_Test_Type type;             /**< The type of test */
        int (*func)(Ewl_Container *con);/**< The test function */
        Ewl_Unit_Test *unit_tests;      /**< The test unit tests */
};

#endif

