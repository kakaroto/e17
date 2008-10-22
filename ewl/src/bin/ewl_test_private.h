/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_TEST_PRIVATE_H
#define EWL_TEST_PRIVATE_H

#include <ewl-config.h>

#define LOG_FAILURE(buffer, len, format, args...) \
        snprintf(buf, len, "%s:%d " format, __FILE__, __LINE__, ## args)

#endif /* EWL_TEST_PRIVATE_H */
