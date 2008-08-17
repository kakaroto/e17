#include "Elation.h"

#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>
#include <sys/types.h>
#include <dirent.h>

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
   
   char *mount_dir;
   char *device;
   
   unsigned char have_media : 1;
   unsigned char check_media_done : 1;
};

#define CMD_CHECK 1
#define CMD_EJECT 2

#define INF_NO_MEDIA 1
#define INF_MEDIA    2
#define TYPE_UNKNOWN 3
#define TYPE_AUDIO   4
#define TYPE_VCD     5
#define TYPE_SVCD    6
#define TYPE_DVD     7
#define TYPE_DATA    8
#define TYPE_MIXED   9
#define TYPE_BLANK   10

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

static char *disk_mount(char *dev);
static void  disk_unmount(char *mount_dir);
static int   disk_has_top_dir(char *mount_dir, char *dir);
    
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

   pr->device = strdup("/dev/cdrom");
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
		  int type = 0;
		  int buf;
		  
		  read(pr->slave_side_read_fd, &buf, sizeof(buf));
		  if (buf == CMD_CHECK)
		    {
		       int fd;
		       int ok = 0;
		       
		       printf("check...\n");
		       if (pr->have_media) continue;
		       fd = open(pr->device, O_RDONLY | O_NONBLOCK);
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
				 printf("first check: %i\n", ok);
				 pr->check_media_done = 1;
			      }
			    close(fd);
			 }
		       if (ok)
			 {
			    int fd;
			    
			    pr->have_media = 1;
			    printf("have media\n");
			    /* FIXME: this is where we should check what kind of media we have */
			    fd = open(pr->device, O_RDONLY | O_NONBLOCK);
			    if (fd >= 0)
			      {
				 int ret;
				 
				 ret = ioctl(fd, CDROM_DRIVE_STATUS, CDSL_CURRENT);
				 if (ret == CDS_DISC_OK)
				   {
				      struct cdrom_tochdr hd;
				      
				      ret = ioctl(fd, CDROMREADTOCHDR, &hd);
				      if (ret != 0) /* blank */
					type = 7;
				      else
					{
					   ret = ioctl(fd, CDROM_DISC_STATUS, CDSL_CURRENT);
					   if (ret == CDS_AUDIO)
					     type = 1;
					   else if (ret == CDS_MIXED)
					     type = 6;
					   else if ((ret == CDS_DATA_1) ||
						    (ret == CDS_DATA_2))
					     {
						pr->mount_dir = disk_mount(pr->device);
						if (pr->mount_dir)
						  {
						     if (disk_has_top_dir(pr->mount_dir, "video_ts"))
						       type = 4;
						     else if (disk_has_top_dir(pr->mount_dir, "vcd"))
						       type = 2;
						     else if (disk_has_top_dir(pr->mount_dir, "svcd"))
						       type = 3;
						     else
						       type = 5;
						  }
					     }
					}
				   }
				 close(fd);				 
			      }
			 }
		       else
			 pr->have_media = 0;
		       if (pr->have_media) buf = INF_MEDIA;
		       else buf = INF_NO_MEDIA;
		       write(pr->slave_side_write_fd, &buf, sizeof(buf));
		       if (type == 0) /* unknown */
			 buf = TYPE_UNKNOWN;
		       else if (type == 1) /* audio */
			 buf = TYPE_AUDIO;
		       else if (type == 2) /* vcd */
			 buf = TYPE_VCD;
		       else if (type == 3) /* svcd */
			 buf = TYPE_SVCD;
		       else if (type == 4) /* dvd */
			 buf = TYPE_DVD;
		       else if (type == 5) /* data */
			 buf = TYPE_DATA;
		       else if (type == 6) /* mixed */
			 buf = TYPE_MIXED;
		       else if (type == 7) /* mixed */
			 buf = TYPE_BLANK;
		       printf("disk type: %i\n", type);
		       write(pr->slave_side_write_fd, &buf, sizeof(buf));
		    }
		  else if (buf == CMD_EJECT)
		    {
		       int fd;
		       
		       printf("eject..\n");
		       if (pr->mount_dir)
			 {
			    disk_unmount(pr->mount_dir);
			    free(pr->mount_dir);
			 }
		       pr->have_media = 0;
		       fd = open(pr->device, O_RDONLY | O_NONBLOCK);
		       if (fd >= 0)
			 {
			    int i;
			    printf("eject disk!\n");
			    
			    for (i = 0; i < 10; i++)
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
   free(pr->device);
   if (pr->mount_dir)
     {
	disk_unmount(pr->mount_dir);
	free(pr->mount_dir);
     }
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
	     else if (buf == TYPE_UNKNOWN)
	       em->info->func.action_broadcast(ELATION_ACT_DISK_TYPE_UNKNOWN);
	     else if (buf == TYPE_AUDIO)
	       em->info->func.action_broadcast(ELATION_ACT_DISK_TYPE_AUDIO);
	     else if (buf == TYPE_VCD)
	       em->info->func.action_broadcast(ELATION_ACT_DISK_TYPE_VCD);
	     else if (buf == TYPE_SVCD)
	       em->info->func.action_broadcast(ELATION_ACT_DISK_TYPE_SVCD);
	     else if (buf == TYPE_DVD)
	       em->info->func.action_broadcast(ELATION_ACT_DISK_TYPE_DVD);
	     else if (buf == TYPE_DATA)
	       em->info->func.action_broadcast(ELATION_ACT_DISK_TYPE_DATA);
	     else if (buf == TYPE_MIXED)
	       em->info->func.action_broadcast(ELATION_ACT_DISK_TYPE_MIXED);
	     else if (buf == TYPE_BLANK)
	       em->info->func.action_broadcast(ELATION_ACT_DISK_TYPE_BLANK);
	  }
     }
   return 1;
}

static char *
disk_mount(char *dev)
{
   FILE *f;
   char buf[4096];
   char realdev[PATH_MAX];
   char *mountpoint = NULL;
   
   if (!realpath(dev, realdev)) return NULL;
   f = fopen("/etc/fstab", "rb");
   while (fgets(buf, sizeof(buf), f))
     {
	char tdev[PATH_MAX];
	char realtdev[PATH_MAX];
	char mount[PATH_MAX];
	
	if (buf[0] != '#')
	  {
	     sscanf(buf, "%s %s", tdev, mount);
	     if (realpath(tdev, realtdev))
	       {
		  if (!strcmp(realtdev, realdev))
		    {
		       mountpoint = strdup(mount);
		       break;
		    }
	       }
	  }
     }
   fclose(f);
   if (mountpoint)
     {
	snprintf(buf, sizeof(buf), "mount %s", mountpoint);
	system(buf);
     }
   return mountpoint;
}

static void
disk_unmount(char *mount_dir)
{
   if (mount_dir)
     {
	char buf[4096];
	
	snprintf(buf, sizeof(buf), "umount %s", mount_dir);
	system(buf);
     }
}

static int
disk_has_top_dir(char *mount_dir, char *dir)
{
   DIR                *dirp;
   struct dirent      *dp;
   
    dirp = opendir(mount_dir);
   if (!dirp) return 0;
   while ((dp = readdir(dirp)))
     {
	if ((strcmp(dp->d_name, ".")) && (strcmp(dp->d_name, "..")))
	  {
	     char buf[PATH_MAX];
	     
	     strcpy(buf, dp->d_name);
	     if (!strcasecmp(dp->d_name, dir))
	       {
		  closedir(dirp);
		  return 1;
	       }
	  }
     }
   closedir(dirp);
   return 0;
}
