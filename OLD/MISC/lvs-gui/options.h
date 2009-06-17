/**********************************************************************
 * options.h                                              December 1999
 *
 * Read in command line options
 *
 * Copyright (C) 1999 Carsten Haitzler and Simon Horman
 *  
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *   
 * The above copyright notice and this permission notice shall be
 * included in all copies of the Software, its documentation and
 * marketing & publicity materials, and acknowledgment shall be given
 * in the documentation, materials and software packages that this
 * Software was used.
 *    
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **********************************************************************/

#ifndef PERDITION_OPT_STIX
#define PERDITION_OPT_STIX

#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>   /*For u_int32_t */

#ifdef HAVE_CONFIG_H
#include "../config.h"
#else
#define HAVE_GETOPT_LONG /*Comment this out if you do not have getopt_long*/
#endif

#define MAX_LINE_LENGTH 4096

typedef u_int32_t flag_t;

#define DEFAULT_MASTER_HOST                    "localhost"
#define DEFAULT_IPVS_CONFIG_FILE               "/etc/sysconfig/ipvs"
#define DEFAULT_IPVS_INIT_SCRIPT               "/etc/rc.d/init.d/ipvs"
#define DEFAULT_TRANSPARENT_PROXY_CONFIG_FILE \
  "/etc/sysconfig/transparent_proxy"
#define DEFAULT_TRANSPARENT_PROXY_INIT_SCRIPT \
  "/etc/rc.d/init.d/transparent_proxy"
#define DEFAULT_RSH_COMMAND                    "ssh -q -x -a"
#define DEFAULT_RCP_COMMAND                    "scp -q"
#define DEFAULT_RC_FILE                        ".lvs-gui.rc"
#define DEFAULT_USER                           "root"

typedef struct {
  char            *master_host;
  char            *ipvs_config_file;
  char            *ipvs_init_script;
  char            *transparent_proxy_config_file;
  char            *transparent_proxy_init_script;
  char            *rsh_command;
  char            *rcp_command;
  char            *rc_file;
  char            *user;
  flag_t          mask;
} options_t;

/*options_t.mask entries*/
#define MASK_MASTER_HOST                   (flag_t) 0x00000001
#define MASK_IPVS_CONFIG_FILE              (flag_t) 0x00000002
#define MASK_IPVS_INIT_SCRIPT              (flag_t) 0x00000004
#define MASK_TRANSPARENT_PROXY_CONFIG_FILE (flag_t) 0x00000008
#define MASK_TRANSPARENT_PROXY_INIT_SCRIPT (flag_t) 0x00000010
#define MASK_RSH_COMMAND                   (flag_t) 0x00000020
#define MASK_RCP_COMMAND                   (flag_t) 0x00000040
#define MASK_RC_FILE                       (flag_t) 0x00000080
#define MASK_USER                          (flag_t) 0x00000100

/*Flag values for options()*/
#define OPT_ERR         (flag_t) 0x1  /*Print error to stderr, enable help*/
#define OPT_CLEAR_MASK  (flag_t) 0x2  /*Set mask to 0*/
#define OPT_SET_MASK    (flag_t) 0x4  /*Add to mask as options are set*/
#define OPT_USE_MASK    (flag_t) 0x8  /*Don't accept options in the mask*/
#define OPT_SET_DEFAULT (flag_t) 0x10 /*Reset options to defaults before
                                           reading options passed*/
#define OPT_FILE        (flag_t) 0x20 /*Reading an options file*/
#define OPT_NOT_SET     (flag_t) 0x40 /*Option is not set, don't free*/
#define OPT_LIT         (flag_t) 0x80 /*Option is a litteral, don't free
                                            or copy, over-rides OPT_SET*/

#define OPT_FIRST_CALL  (flag_t) \
 OPT_ERR|OPT_CLEAR_MASK|OPT_SET_MASK|OPT_SET_DEFAULT


/**********************************************************************
 * options
 * Read in command line options
 * pre: argc: number or elements in argv
 *      argv: array of strings with command line-options
 *      flag: see options.h for flag values
 *            ignores errors otherwise
 * post: global opt is seeded with values according to argc and argv
 **********************************************************************/

int options(int argc, char **argv, flag_t flag);


/**********************************************************************
 * usage
 * Display usage information
 * Printed to stdout if exit_status=0, stderr otherwise
 **********************************************************************/

void usage(int exit_status);


/**********************************************************************
 * options_to_stream
 * Write options with comments to a stream
 * pre: stream: open stream to write to
 * post: options are writen to stream
 * return: return from the single fprintf used
 **********************************************************************/

int options_to_stream(FILE *stream);

#endif
