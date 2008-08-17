/**********************************************************************
 * config_file.h                                          November 1999
 *
 * Read in a config and parse it into command line arguments,
 * return this as a dynamic array
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


#ifndef CONFIG_FILE_BERT
#define CONFIG_FILE_BERT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "options.h"
#include "dynamic_array.h"

#define MAX_LINE_LENGTH 4096

/* Flags for config_file_to_opt */
#define CONFIG_FILE_ERR (flag_t) 0x1    /*Log errors to stderr*/


/**********************************************************************
 * config_file_to_opt
 * Configure opt structre according to options specified in a config
 * file.
 * pre: filename: file to read options from
 * post: options in global options_t opt are set according to
 *       config file. Options specified onthe command line
 *       override config file options
 **********************************************************************/
void config_file_to_opt(const char *filename);


/**********************************************************************
 * config_file_read
 * Read in a config file and put elements in a dynamic array
 * pre: filename: file to read configuration from
 * return: dynamic array containin elements, keys are preceded by
 *         a -- and must be long opts as per options.c.
 *         If a key is a single letter it is preceded by a -
 *         and must be a short opt as per options.c
 *         A key is the first whitespace delimited word on a line
 *         Blank lines are ignored.
 *         Everthing including and after a hash (#) on a line is 
 *         ignored
 **********************************************************************/

dynamic_array_t *config_file_read (const char *filename);

/**********************************************************************
 * config_file_write
 * Write options to a file
 * pre: filename file to write to
 * post: options are written to config file
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

int config_file_write(const char *filename);

#endif
