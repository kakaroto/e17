/**********************************************************************
 * config_file.c                                          November 1999
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


#include "config_file.h"

/**********************************************************************
 * config_file_to_opt
 * Configure opt structire according to options specified in a config
 * file.
 * pre: filename: file to read options from
 * post: options in global options_t opt are set according to
 *       config file. Options specified onthe command line
 *       override config file options
 **********************************************************************/

void config_file_to_opt(const char *filename){
  dynamic_array_t *a;

  if((a=config_file_read(filename))==NULL){
    return;
  }
  
  /* Set options according to config file but only if they are not 
   * Masked (overriden on the command line)
   */

  options(
    dynamic_array_get_count(a),
    dynamic_array_get_vector(a),
    OPT_USE_MASK|OPT_FILE|OPT_ERR
  );

  dynamic_array_destroy(a, DESTROY_STR);

  return;
}


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

#define ADD_ELEMENT\
  if((a=dynamic_array_add_element( \
    a,  \
    buffer+(token==0?((*(buffer+3)=='\0')?1:0):2), \
    DESTROY_STR, \
    DUP_STR \
  ))==NULL){ \
    fprintf(stderr, "config_file_read: dynamic_array_add_element\n"); \
    return(NULL); \
  }


dynamic_array_t *config_file_read (const char *filename){
  char key[MAX_LINE_LENGTH];
  char tail[MAX_LINE_LENGTH];
  char buffer[MAX_LINE_LENGTH];
  char format[MAX_LINE_LENGTH];
  int status;
  int token=0;
  int comment=0;
  int offset=0;
  char *s;
  dynamic_array_t *a;
  FILE *stream;

  extern int errno;

  if(filename==NULL) return(NULL);
  if((stream=fopen(filename, "r"))==NULL){
    return(NULL);
  }

  if((a=dynamic_array_create((size_t)0))==NULL){
    fprintf(stderr, "config_file_read: dynamic_array_create\n");
    return(NULL);
  }
  /*insert a dummy argv[0] into the dynamic array*/
  if((a=dynamic_array_add_element(a, "", DESTROY_STR, DUP_STR))==NULL){
    fprintf(stderr, "config_file_read: dynamic_array_add_element\n");
    return(NULL);
  }

  *key=*(key+1)=*buffer=*(buffer+1)='-';
  sprintf(
    format,
    "%%%d[^ \t\n\r#]%%%d[ \t\n\r#]",
    MAX_LINE_LENGTH-3, 
    MAX_LINE_LENGTH-1
  );
  while((status=fscanf(stream, format, key+2+offset, tail))!=EOF){
    if(status==0) { status=fscanf(stream, "%[^\n]\n", tail); continue; }
    if(!comment){
      strncpy(buffer+offset,key+offset,MAX_LINE_LENGTH-offset);
      if(token==0){
        ADD_ELEMENT;
      }
    }
    if(status==2){
      if((s=strrchr(tail, '\n'))==NULL){
        if(token++>0){
          offset=strlen(key+2);
          *(key+2+offset)=' ';
          *(buffer+2+offset++)=' ';
        }
      }
      else{
        if(!comment && *buffer!='\0'){
          ADD_ELEMENT;
        }
        offset=0;
        token=0;
        comment=0;
        *buffer='\0';
      }
      if(!comment){
        comment=(strchr(s!=NULL?s:tail, '#')==NULL)?0:1;
      }
    }
  }
  return(a);
}


/**********************************************************************
 * config_file_write
 * Write options to a file
 * pre: filename file to write to
 * post: options are written to config file
 * return: 0 on success
 *         -1 on error
 **********************************************************************/

int config_file_write(const char *filename){
  FILE *stream;

  if((stream=fopen(filename, "w"))==NULL){
    return(-1);
  }
  if(options_to_stream(stream)<0){
    return(-1);
  }
  fclose(stream);

  return(0);
}
 
