#ifndef _EWD_H
#define _EWD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#include <ewd_config.h>
#include <ewd_macros.h>
#include <ewd_threads.h>
#include <ewd_value.h>
#include <ewd_list.h>
#include <ewd_tree.h>
#include <ewd_hash.h>
#include <ewd_sheap.h>
#include <ewd_path.h>
#include <ewd_plugin.h>
#include <ewd_strings.h>

#endif
