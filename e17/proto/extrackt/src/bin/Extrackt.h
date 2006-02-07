/* vim: set ts=8 sw=3 sts=8 noexpandtab cino=>5n-3f0^-2{2: */
#ifndef _EXTRACKT_H
#define _EXTRACKT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <config.h>

#include <Eet.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_File.h>
#include <Enhance.h>

#include "config.h"

#define TRUE 1
#define FALSE 0

#define ERROR(string) \
     fprintf(stderr,"[extrackt error]: "#string"\n");
#define DEBUG(string) \
   fprintf(stdout,"[extrackt debug]: "#string"\n");

#ifndef ENHANCE_MEM_DEBUG

#ifndef E_FREE
#define E_FREE(ptr) if(ptr) { free (ptr); ptr = NULL;}
#endif

#ifndef E_NEW
#define E_NEW(count, type) calloc(count, sizeof(type))
#endif

#ifndef E_STRDUP
#define E_STRDUP(str) strdup(str)
#endif

#endif

#define CDROM_DEV ""

#ifdef HAVE_LINUX_CDROM_H
#undef CDROM_DEV
#define CDROM_DEV "/dev/cdrom"
#endif

#ifdef HAVE_SYS_CDIO_H
#undef CDROM_DEV
#define CDROM_DEV "/dev/acd0"
#endif

typedef struct _Extrackt_Frontend Extrackt_Frontend;
typedef struct _Extrackt Extrackt;
typedef enum   _Ex_Status Ex_Status;

#include "util.h"
#include "cddev.h"
#include "conf.h"
#include "cddb.h"
#include "command.h"
#include "main.h"
#include "protocol.h"
#include "http.h"

enum _Ex_Status
{
   EX_STATUS_DONE,
   EX_STATUS_DOING,
   EX_STATUS_NOT_DONE,		/* this is redundant as the others are mutual exclusive */
   EX_STATUS_CANT_BE_DONE	/* if theres an error on the cddb or something dont recall it */
};

struct _Extrackt_Frontend
{
	/* for now all are just receive/returns void to simplify
	 * we'll se later what params would need */
	void (*init)(Extrackt *);	
	void (*main)(void *);	/* the main loop */
	void (*shutdown)(void *);
	void (*disc_update)(void *);
	void (*rip_percent_update) (double percent);
	
	void *data;		/* pointer to gui's struct */
};

struct _Extrackt
{
   Extrackt_Frontend *fe;
   int               busy;        /* is app ripping / encoding ? */
   int               disc_id;     /* our own disc (cddb) id */
   int 		     pfd[2];      /* pipe where external proc writes */
   int 		     dfd;         /* the fd for the cd device */
   
   struct {
      pid_t	      pid;	  /* pid cddb lookup*/
      Ex_Status       status;
      Ex_Cddb_Hello  *hello;
   } cddb;
   
   struct {
      pid_t	     pid;	  /* pid ripper */
      Ex_Status	     status;
      Ex_Config_Exe *ripper;
      Evas_List	    *tracks;
      int	     num_total;
      int	     num_done;
      time_t	     start;
      
      struct {
	 int	     number;
	 char	    *filename;
	 long int  size;	   
      } curr_track;
   } rip;
   
   struct {
      int	     on;
      pid_t	     pid;     /* pid encoder */
      Ex_Status      status;
      Ex_Config_Exe *encoder;
      Evas_List     *tracks;   /* a list of filenames to encode */
      int	     num_queue;
      int	     num_total;
      int	     num_done;
      time_t	     start;

      struct {
	 int	     number;
	 char	    *filename;
	 long int  size;	 
      } curr_track;            
   } encode;
   
   Ex_Disc_Info disc_info;
   Ex_Disc_Data disc_data;        /* FIXME this should be dynamic, for now only one match */
   
   struct {
      Ex_Config_Version *version;
      Ex_Config_Cd      *cd;      /* config struct for cdrom */
      Ex_Config_Cddb    *cddb;
      Ex_Config_Encode  *encode;
      Evas_List         *rippers; /* config structs for rippers */
   } config;
};

/* THIS IS A TEMP!! */
void Debug(char *fmt,...);

/* This is here because we dont use gettext for now */
#undef _
#define _(arg) arg

#endif
