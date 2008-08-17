/*
 * $Id$
 * vim:expandtab:ts=3:sts=3:sw=3
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <../config.h>
#include "envision.h"

#define OPTSTR "v?he:"

static struct option long_options[] = {
   {"help", 0, 0, '?'},
   {"version", 0, 0, 'v'},
   {"engine", 0, 0, 'e'}
};

extern char *optarg;

int envision_config_get(Config * config, int argc, char *argv[]);
int envision_parse_usage(Config * config, int argc, char *argv[]);
int envision_parse_config(Config * config);

void printf_usage(char *argv[]);

#endif /* __CONFIG_H */
