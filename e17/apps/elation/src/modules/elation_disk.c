#include "Elation.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>

/* external module symbols. the rest is private */
void *init(Elation_Module *em);

/********************/
typedef struct _Elation_Module_Private Elation_Module_Private;

struct _Elation_Module_Private
{
   Ecore_Timer *media_check_timer;
   
   int   slave_read_fd;
   int   slave_write_fd;
   int   slave_side_read_fd;
   int   slave_side_write_fd;
   pid_t slave_pid;
   
   Ecore_Fd_Handler *slave_fd_handler;
   
   unsigned char have_media : 1;
   unsigned char check_media_done : 1;
};

#define CMD_CHECK 1
#define CMD_EJECT 2

#define INF_NO_MEDIA 1
#define INF_MEDIA    2

static void shutdown(Elation_Module *em);
static void resize(Elation_Module *em);
static void show(Elation_Module *em);
static void hide(Elation_Module *em);
static void focus(Elation_Module *em);
static void unfocus(Elation_Module *em);
static void action(Elation_Module *em, int action);

static int  media_check_timer_cb(void *data);
static void media_eject(Elation_Module *em);
static int  slave_fd_cb(void *data, Ecore_Fd_Handler *fdh);
    
void *
init(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = calloc(1, sizeof(Elation_Module_Private));
   if (!pr) return NULL;
   
   /* set up module methods */
   em->shutdown = shutdown;
   em->resize = resize;
   em->show = show;
   em->hide = hide;
   em->focus = focus;
   em->unfocus = unfocus;
   em->action = action;

     {
	int fds_in[2], fds_out[2];
	pid_t pid;
	
	if (pipe(fds_in) != 0)
	  perror("pipe");
	if (pipe(fds_out) != 0)
	  perror("pipe");
	
	pr->slave_read_fd = fds_in[0];
	pr->slave_write_fd = fds_out[1];
	pr->slave_side_read_fd = fds_out[0];
	pr->slave_side_write_fd = fds_in[1];
	  
	fcntl(pr->slave_read_fd, F_SETFL, O_NONBLOCK);
	fcntl(pr->slave_write_fd, F_SETFL, O_NONBLOCK);
	
	pr->slave_fd_handler = ecore_main_fd_handler_add(pr->slave_read_fd,
							 ECORE_FD_READ,
							 slave_fd_cb,
							 em,
							 NULL,
							 NULL);
	ecore_main_fd_handler_active_set(pr->slave_fd_handler, ECORE_FD_READ);
	
	pid = fork();
	if (pid == 0)
	  {
	     for (;;)
	       {
		  int buf;
		  
		  read(pr->slave_side_read_fd, &buf, sizeof(buf));
		  if (buf == CMD_CHECK)
		    {
		       int fd;
		       int ok = 0;
		       
		       printf("check...\n");
		       if (pr->have_media) continue;
		       fd = open("/dev/dvd", O_RDONLY | O_NONBLOCK);
		       if (fd >= 0)
			 {
			    if (pr->check_media_done)
			      {
				 struct cdrom_generic_command cgc;
				 struct request_sense sense;
				 unsigned char buffer[8];
				 int ret;
				 
				 memset(&sense, 0, sizeof(sense));
				 cgc.cmd[0] = GPCMD_GET_EVENT_STATUS_NOTIFICATION;
				 cgc.cmd[1] = 1;
				 cgc.cmd[4] = 16;
				 cgc.cmd[8] = sizeof(buffer);
				 cgc.timeout = 600;
				 cgc.buffer = buffer;
				 cgc.buflen = sizeof(buffer);
				 cgc.data_direction = CGC_DATA_READ;
				 cgc.sense = &sense;
				 cgc.quiet = 1;
				 ret = ioctl(fd, CDROM_SEND_PACKET, &cgc);
				 if (ret == -1)
				   ok = 0;
				 else
				   {
				      int val;
				      
				      val = buffer[4] & 0xf;
				      /* 3 = eject request */
				      /* 2 = inseted new disk */
				      if ((val == 2) || (val == 4)) ok = 1;
				   }
			      }
			    else
			      {
				 unsigned char buffer[8];
				 int ret;
				 
				 ret = read(fd, buffer, 8);
				 if (ret == -1)
				   ok = 0;
				 else
				   ok = 1;
				 pr->check_media_done = 1;
			      }
			    close(fd);
			 }
		       if (ok)
			 {
			    pr->have_media = 1;
			    printf("have media\n");
			    /* FIXME: this is where we should check what kind of media we have */
			 }
		       else
			 pr->have_media = 0;
		       if (pr->have_media) buf = INF_MEDIA;
		       else buf = INF_NO_MEDIA;
		       write(pr->slave_side_write_fd, &buf, sizeof(buf));
		    }
		  else if (buf == CMD_EJECT)
		    {
		       int fd;
		       
		       printf("eject..\n");
		       pr->have_media = 0;
		       fd = open("/dev/dvd", O_RDONLY | O_NONBLOCK);
		       if (fd >= 0)
			 {
			    int i;
			    printf("eject disk!\n");
			    
			    for (i = 0; i < 5; i++)
			      {
				 if (ioctl(fd, CDROMEJECT, 0) == 0) break;
				 perror("ioctl");
				 sleep(1);
			      }
			    close(fd);
			 }
		    }
	       }
	     exit(0);
	  }
     }
   
   pr->media_check_timer = ecore_timer_add(0.5, media_check_timer_cb, em);
   
   return pr;
}

static void
shutdown(Elation_Module *em)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   if (pr->media_check_timer) ecore_timer_del(pr->media_check_timer);
   close(pr->slave_read_fd);
   kill(pr->slave_pid, SIGKILL);
   free(pr);
}

static void
resize(Elation_Module *em)
{
}

static void
show(Elation_Module *em)
{
}

static void
hide(Elation_Module *em)
{
}

static void
focus(Elation_Module *em)
{
}

static void
unfocus(Elation_Module *em)
{
}
   
static void
action(Elation_Module *em, int action)
{
   Elation_Module_Private *pr;
   
   pr = em->data;
   switch (action)
     {
      case ELATION_ACT_DISK_EJECT:
	media_eject(em);
	break;
      default:
	break;
     }
}

/*** private stuff ***/

static int
media_check_timer_cb(void *data)
{
   Elation_Module *em;
   Elation_Module_Private *pr;
   int buf;
   
   em = data;
   pr = em->data;

   buf = CMD_CHECK;
   write(pr->slave_write_fd, &buf, sizeof(buf));
   return 1;
}

static void
media_eject(Elation_Module *em)
{
   Elation_Module_Private *pr;
   int buf;         
   
   pr = em->data;
   buf = CMD_EJECT;
   write(pr->slave_write_fd, &buf, sizeof(buf));
}

static int
slave_fd_cb(void *data, Ecore_Fd_Handler *fdh)
{
   Elation_Module *em;
   Elation_Module_Private *pr;
   int buf, len, fd;
   
   em = data;
   pr = em->data;
   fd = ecore_main_fd_handler_fd_get(fdh);
   while ((len = read(fd, &buf, sizeof(buf))) > 0)
     {
	if (len == sizeof(buf))
	  {
	     if (buf == INF_NO_MEDIA)
	       em->info->func.action_broadcast(ELATION_ACT_DISK_OUT);
	     else if (buf == INF_MEDIA)
	       em->info->func.action_broadcast(ELATION_ACT_DISK_IN);
	  }
     }
   return 1;
}
