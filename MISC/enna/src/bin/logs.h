#ifndef __ENNA_LOGS_H__
#define __ENNA_LOGS_H__

#include "enna.h"

void elog(int level, const char *module, char *file, int line,
        const char *format, ...);

#define enna_log(level,module,fmt,arg...) \
        elog(level,module,__FILE__,__LINE__,fmt,##arg)

#endif /* __ENNA_LOGS_H__ */
