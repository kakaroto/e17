/**********************************************************************
 * options.c                                              December 1999
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

#include "options.h"

options_t opt;


/**********************************************************************
 * options_set_mask
 * Set the options mask
 * pre: mask: pointer to current mask that may be modified
 *      mask_entry: value to or with opt->mask
 *      flag: flags
 * post: mask is added if flags permit
 * return: 1 if mask is added
 *         0 otherwise
 **********************************************************************/

static int options_set_mask(flag_t *mask, flag_t mask_entry, flag_t flag){
  if(flag&OPT_USE_MASK && (*mask)&mask_entry) return(0);
  if(flag&OPT_SET_MASK) (*mask)|=mask_entry;
  return(1);
}


/***********************************************************************
 * opt_p
 * Assign an option that is a char *
 * pre: opt: option to assign
 *      value: value to copy into opt
 *      mask:  current option mask
 *      mask_entry: entry of this option in the option mask
 *      flag:  flags as per options.h
 * post: If the mask and options allow as per options_set_mask()
 *       value is copied into opt. Any existing value of opt is freed
 *       The mask may also be altered as per options_set_mask()
 *       Else no change.
 ***********************************************************************/

#define opt_p(opt, value, mask, mask_entry, flag) \
  if(options_set_mask(&(mask), flag, mask_entry)){ \
    if(!((flag)&OPT_NOT_SET) && opt!=NULL){ free(opt); } \
    opt=(value==NULL)?NULL:strdup(value); \
  }


/***********************************************************************
 * opt_i
 * Assign an option that is an int
 * pre: opt: option to assign
 *      value: value to assign to opt
 *      mask:  current option mask
 *      mask_entry: entry of this option in the option mask
 *      flag:  flags as per options.h
 * post: If the mask and options allow as per options_set_mask()
 *       value is assigned to opt. 
 *       The mask may also be altered as per options_set_mask()
 *       Else no change.
 ***********************************************************************/

#define opt_i(opt, value, mask, mask_entry, flag) \
  if(options_set_mask(&(mask), flag, mask_entry)){ \
    opt=value; \
  }


/**********************************************************************
 * opt_help
 * Display help, only if we are in ERR mode
 **********************************************************************/

#define opt_help \
  if(f&OPT_ERR && !(f&OPT_FILE)){ usage(0); } \
  break;


/**********************************************************************
 * opt_master_host
 * Set master_host
 **********************************************************************/

#define opt_master_host \
  opt_p(opt.master_host,optarg,opt.mask,MASK_MASTER_HOST,f); \
  break;


/**********************************************************************
 * opt_ipvs_config_file
 * Set ipvs_config_file
 **********************************************************************/

#define opt_ipvs_config_file \
  opt_p(opt.ipvs_config_file,optarg,opt.mask,MASK_IPVS_CONFIG_FILE,f); \
  break;


/**********************************************************************
 * opt_ipvs_init_script
 * Set ipvs_init_script
 **********************************************************************/

#define opt_ipvs_init_script \
  opt_p(opt.ipvs_init_script,optarg,opt.mask,MASK_IPVS_INIT_SCRIPT,f); \
  break;

/**********************************************************************
 * opt_transparent_proxy_config_file
 * Set transparent_proxy_config_file
 **********************************************************************/

#define opt_transparent_proxy_config_file \
  opt_p( \
    opt.transparent_proxy_config_file, \
    optarg, \
    opt.mask, \
    MASK_TRANSPARENT_PROXY_CONFIG_FILE, \
    f \
  ); \
  break;


/**********************************************************************
 * opt_transparent_proxy_init_script
 * Set transparent_proxy_init_script
 **********************************************************************/

#define opt_transparent_proxy_init_script \
  opt_p( \
    opt.transparent_proxy_init_script, \
    optarg, \
    opt.mask, \
    MASK_TRANSPARENT_PROXY_INIT_SCRIPT, \
    f \
  ); \
  break;


/**********************************************************************
 * opt_rsh_command
 * Set rsh_command
 **********************************************************************/

#define opt_rsh_command \
  opt_p(opt.rsh_command,optarg,opt.mask,MASK_RSH_COMMAND,f); \
  break;


/**********************************************************************
 * opt_rcp_command
 * Set rcp_command
 **********************************************************************/

#define opt_rcp_command \
  opt_p(opt.rcp_command,optarg,opt.mask,MASK_RCP_COMMAND,f); \
  break;


/**********************************************************************
 * opt_user
 * Set user
 **********************************************************************/

#define opt_user \
  opt_p(opt.user,optarg,opt.mask,MASK_USER,f); \
  break;


/**********************************************************************
 * options
 * Read in command line options
 * pre: argc: number or elements in argv
 *      argv: array of strings with command line-options
 *      flag: see options.h for flag values
 *            ignores errors otherwise
 * post: global opt is seeded with values according to argc and argv
 **********************************************************************/

int options(int argc, char **argv, flag_t f){
  int c;
  char * short_options_string="c:hI:i:m:s:T:t:u:";

  extern options_t opt;
  extern int optind;

  /* Re-process the arguments each time options is called*/
  optind = 0;

  if(argc==0 || argv==NULL) return(0);

  /* c is used as a dummy variable */
  if(f&OPT_SET_DEFAULT){
    opt_p(opt.master_host, DEFAULT_MASTER_HOST, c, 0, OPT_NOT_SET);
    opt_p(opt.ipvs_config_file, DEFAULT_IPVS_CONFIG_FILE, c, 0, OPT_NOT_SET);
    opt_p(opt.ipvs_init_script, DEFAULT_IPVS_INIT_SCRIPT, c, 0, OPT_NOT_SET);
    opt_p(
      opt.transparent_proxy_config_file, 
      DEFAULT_TRANSPARENT_PROXY_CONFIG_FILE, 
      c, 
      0, 
      OPT_NOT_SET
    );
    opt_p(
      opt.transparent_proxy_init_script, 
      DEFAULT_TRANSPARENT_PROXY_INIT_SCRIPT, 
      c, 
      0, 
      OPT_NOT_SET
    );
    opt_p(opt.rsh_command, DEFAULT_RSH_COMMAND, c, 0, OPT_NOT_SET);
    opt_p(opt.rcp_command, DEFAULT_RCP_COMMAND, c, 0, OPT_NOT_SET);
    opt_p(opt.user, DEFAULT_USER, c, 0, OPT_NOT_SET);
  }

  if(f&OPT_CLEAR_MASK) opt.mask=(flag_t)0;

  while (1){
#ifdef HAVE_GETOPT_LONG
    int option_index = 0;
    static struct option long_options[] =
    {
      {"master_host",                   1, 0, 0},
      {"ipvs_config_file",              1, 0, 0},
      {"ipvs_init_script",              1, 0, 0},
      {"transparent_proxy_config_file", 1, 0, 0},
      {"transparent_proxy_init_script", 1, 0, 0},
      {"rsh_command",                   1, 0, 0},
      {"rcp_command",                   1, 0, 0},
      {"user",                          1, 0, 0},
      {0, 0, 0, 0}
    };

    c = getopt_long (
      argc, 
      argv, 
      short_options_string, 
      long_options, 
      &option_index
    );
#else
    c = getopt (argc, argv, short_options_string);
#endif
    if (c == -1){
      return(0);
    }

    switch (c){
#ifdef HAVE_GETOPT_LONG
      case 0:
        if(strcmp(long_options[option_index].name,"help")==0){
          opt_help;
        }
        if(strcmp(long_options[option_index].name,"master_host")==0){
          opt_master_host;
        }
        if(strcmp(long_options[option_index].name,"ipvs_config_file")==0){
          opt_ipvs_config_file;
        }
        if(strcmp(long_options[option_index].name,"ipvs_init_script")==0){
          opt_ipvs_init_script;
        }
        if(!strcmp(
          long_options[option_index].name, 
          "transparent_proxy_config_file"
        )){
          opt_transparent_proxy_config_file;
        }
        if(!strcmp(
          long_options[option_index].name, 
          "transparent_proxy_init_script"
        )){
          opt_transparent_proxy_init_script;
        }
        if(!strcmp(long_options[option_index].name, "rsh_command")){
          opt_rsh_command;
        }
        if(!strcmp(long_options[option_index].name, "rcp_command")){
          opt_rcp_command;
        }
        if(!strcmp(long_options[option_index].name, "user")){
          opt_user;
        }
        break;
#endif
      case 'c':
        opt_rcp_command;
      case 'h':
        opt_help;
      case 'I':
        opt_ipvs_init_script;
      case 'i':
        opt_ipvs_config_file;
      case 'T':
        opt_transparent_proxy_config_file;
      case 't':
        opt_transparent_proxy_init_script;
      case 'u':
        opt_user;
      case '?':
        if(f&OPT_ERR){
          usage(-1);
          fprintf(stderr, "options: unknown option: 0%o", optopt);
        }
      default:
        if(f&OPT_ERR){
          fprintf(stderr, "options: getopt returned 0%o", c);
          fprintf(stderr, "options: getopt returned 0%o", c);
        }
      }
  }

  if (optind<argc) {
    if(f&OPT_ERR){
      fprintf(stderr, "options: non-option ARGV-elements");
      usage(-1);
    }
  }

  return(0);
}


/**********************************************************************
 * usage
 * Display usage information
 * Printed to stdout if exit_status=0, stderr otherwise
 **********************************************************************/

void usage(int exit_status){
  FILE *stream;

  if(exit_status!=0){
     stream=stderr;
  }
  else{
     stream=stdout;
  }
  
  fprintf(
    stream, 
    "Usage: perdition [options]\n"
    "  options:\n"
#ifdef HAVE_GETOPT_LONG    
    "     -c|--rcp_command: \n"
    "                      Command to execute to copy files between hosts\n"
    "                      (default \"%s\")\n"
    "     -h|--help:       Display this message\n"
    "     -I|--ipvs_init_script:\n"
    "                      Init sctipt for IPVS\n"
    "                      (default \"%s\")\n"
    "     -i|--ipvs_config_file:\n"
    "                      Config File for IPVS\n"
    "                      (default \"%s\")\n"
    "     -m|--master_host:\n"
    "                      The master host to read and store the\n"
    "                      from and to.\n"
    "                      (default \"%s\")\n"
    "     -s|--rsh_command:\n"
    "                      Command to execute to get a remote shell on hosts\n"
    "                      (default \"%s\")\n"
    "     -T|--transparent_proxy_init_script:\n"
    "                      Init sctipt for transparent proxy\n"
    "                      (default \"%s\")\n"
    "     -t|--transparent_proxy_config_file:\n"
    "                      Config File for transparent proxy\n"
    "                      (default \"%s\")\n"
    "     -u|--user:       User to login as when copying files to and\n"
    "                      executing commands on remote hosts\n"
    "                      (default \"%s\")\n"
#else
    "     -c: Command to execute to copy files between hosts\n"
    "         (default \"%s\")\n"
    "     -h: Display this message\n"
    "     -I: Init sctipt for IPVS\n"
    "         (default \"%s\")\n"
    "     -i: Config File for IPVS\n"
    "         (default \"%s\")\n"
    "     -m: The master host to read and store the from and to\n"
    "         (default \"%s\")\n"
    "     -s: Command to execute to get a remote shell on hosts\n"
    "         (default \"%s\")\n"
    "     -T: Init sctipt for transparent proxy\n"
    "         (default \"%s\")\n"
    "     -t: Config File for transparent proxy\n"
    "         (default \"%s\")\n"
    "     -u: User to login as when copying files to and executing commands\n"
    "         on remote hosts\n"
    "         (default \"%s\")\n"

#endif
    ,
    DEFAULT_RCP_COMMAND,
    DEFAULT_IPVS_INIT_SCRIPT,
    DEFAULT_IPVS_CONFIG_FILE,
    DEFAULT_MASTER_HOST,
    DEFAULT_RSH_COMMAND,
    DEFAULT_TRANSPARENT_PROXY_INIT_SCRIPT,
    DEFAULT_TRANSPARENT_PROXY_CONFIG_FILE,
    DEFAULT_USER
  );

  exit(exit_status);
}
