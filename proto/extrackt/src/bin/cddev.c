/* vim: set ts=8 sw=3 sts=8 noexpandtab cino=>5n-3f0^-2{2: */
/* cddev.c
 * *******
 * 
 * Based on code from libcdaudio 0.5.0 (Copyright (C)1998 Tony Arcieri)
 * Based on code from Grip 3.3.1 (Copyright (C)2004 Mike Oliphant)
 * 
 * functions to poll for tracks of the cd
 * detect a cd
 * read track info
 */

#include "Extrackt.h"

/* We can check to see if the CD-ROM is mounted if this is available */
#ifdef HAVE_MNTENT_H
#include <mntent.h>
#endif

/* For Linux */
#ifdef HAVE_LINUX_CDROM_H
#include <linux/cdrom.h>
#define NON_BLOCKING
#endif

/* For FreeBSD, OpenBSD, and Solaris */
#ifdef HAVE_SYS_CDIO_H
#include <sys/cdio.h>
#define NON_BLOCKING
#endif

#if defined(__FreeBSD__) || defined(__NetBSD__)
    #define CDIOREADSUBCHANNEL CDIOCREADSUBCHANNEL
#endif

/* For Digital UNIX */
#ifdef HAVE_IO_CAM_CDROM_H
#include <io/cam/cdrom.h>
#endif

void Debug(char *fmt,...)
{
   return;
}

void
ex_cddev_update(Extrackt *ex)
{
   long int current_id;
   
   if(!ex_cddev_stat(ex, 1))
     {
	ERROR("Cant stat device!");
	if(ex->cddb.status != EX_STATUS_DOING)
	  {
	     /* kill the cddb child */
	     ex->cddb.status = EX_STATUS_NOT_DONE;
	  }
	return;
     }
   
   current_id = ex_cddb_discid_get(ex);
   
   if(ex->disc_id == current_id && ex->cddb.status != EX_STATUS_NOT_DONE)
     return;
   
   if(ex->cddb.status == EX_STATUS_DOING)
     return;

#if 0   
   if(ex->cddb.status == EX_STATUS_DONE)
     return;
   if(ex->cddb.status == EX_STATUS_CANT_BE_DONE)
     return;
#endif
   
   ex->disc_id = current_id;
   ex_cddb_default_set(&(ex->disc_data), ex->disc_info.num_tracks);
   ex_cddb_match_find(ex);
}

/* subsystem functions */

/* Initialize the CD-ROM for playing audio CDs */
int
ex_cddev_init(char *devname, Extrackt *ex)
{
   struct stat st;
#ifdef HAVE_MNTENT_H
   FILE *mounts;
   struct mntent *mnt;
   char devname[256];
#endif
#ifndef NON_BLOCKING
   const int OPEN_MODE = O_RDONLY;
#else
   const int OPEN_MODE = O_RDONLY|O_NONBLOCK;
#endif
   
   ex->disc_info.have_info=FALSE;
   ex->disc_info.disc_present=FALSE;
   
   if(lstat(devname,&st)<0)
     return FALSE;
   
#ifdef HAVE_MNTENT_H
   if(S_ISLNK(st.st_mode))
     readlink(devname,devname,256);
   else
     strncpy(devname,devname,256);
   
   if((mounts=setmntent(MOUNTED, "r"))==NULL)
     return FALSE;
   
   while((mnt=getmntent(mounts))!=NULL) {
      if(strcmp(mnt->mnt_fsname,devname)==0) {
	 endmntent(mounts);
	 errno = EBUSY;
	 return FALSE;
      }
   }
   
   endmntent(mounts);
#endif
   
   if (ex->config.cd->cdrom 
       && ex->config.cd->cdrom != devname
       && strcmp(devname, ex->config.cd->cdrom)) {
      E_FREE(ex->config.cd->cdrom);
      ex->config.cd->cdrom = 0;
   }
   if (!ex->config.cd->cdrom) {
      ex->config.cd->cdrom = E_STRDUP(devname);
   }
   
   ex->disc_info.cd_desc=open(devname, OPEN_MODE);
   
   if(ex->disc_info.cd_desc==-1) return FALSE;
   
   return TRUE;
}

int 
ex_cddev_close(Extrackt *ex)
{
   close(ex->disc_info.cd_desc);
   ex->disc_info.cd_desc = -1;
   
   return TRUE;
}

int
ex_cddev_stat_new(Extrackt *ex)
{
   return 1;
}
      
/* Read the Table Of Contents header */
int
ex_cddev_toc_read(Extrackt *ex)
{
   
#ifdef CDIOREADSUBCHANNEL
   struct ioc_read_subchannel cdsc;
   struct cd_sub_channel_info data;
#endif
#ifdef CDIOREADTOCENTRYS
   struct cd_toc_entry toc_buffer[MAX_TRACKS];
   struct ioc_read_toc_entry cdte;
#endif
/* UNUSED
#ifdef CDROMSUBCHNL
   struct cdrom_subchnl cdsc;
#endif
*/
#ifdef CDROM_READ_SUBCHANNEL
   struct cd_sub_channel sch;
#endif
#ifdef CDROMREADTOCHDR
   struct cdrom_tochdr cdth;
#endif
#ifdef CDIOREADTOCHEADER   
   struct ioc_toc_header cdth;   
#endif      
#ifdef CDROMREADTOCENTRY
   struct cdrom_tocentry cdte;
#endif   
   
   int readtracks,frame[MAX_TRACKS],pos;
   
#ifdef CDIOREADTOCHEADER
   if(ioctl(ex->disc_info.cd_desc,CDIOREADTOCHEADER,(char *)&cdth)<0) {
      printf(_("Error: Failed to read disc contents\n"));
      
      return FALSE;
   }
   
   ex->disc_info.num_tracks=cdth.ending_track;
#endif
   
#ifdef CDROMREADTOCHDR
      if(ioctl(ex->disc_info.cd_desc,CDROMREADTOCHDR,&cdth)<0) {
	 printf(_("Error: Failed to read disc contents\n"));
	 
	 return FALSE;
      }
      
      ex->disc_info.num_tracks=cdth.cdth_trk1;
#endif
      
      /* Read the table of contents */
      
#ifdef CDIOREADTOCENTRYS
      cdte.address_format=CD_MSF_FORMAT;
      cdte.starting_track=0;
      cdte.data=toc_buffer;
      cdte.data_len=sizeof(toc_buffer);
      
      if(ioctl(ex->disc_info.cd_desc,CDIOREADTOCENTRYS,(char *)&cdte)<0) {
	 printf(_("Error: Failed to read disc contents\n"));
	 
	 return FALSE;
      }
      
      for(readtracks=0;readtracks<=ex->disc_info.num_tracks;readtracks++) {
	 ex->disc_info.track[readtracks].start_pos.mins=
	   cdte.data[readtracks].addr.msf.minute;
	 ex->disc_info.track[readtracks].start_pos.secs=
	   cdte.data[readtracks].addr.msf.second;
	 frame[readtracks]=cdte.data[readtracks].addr.msf.frame;
	 
	 /* I'm just guessing about this based on cdio.h -- should be tested */
	 /* This compiles on freebsd, does it work? */
	 ex->disc_info.track[readtracks].flags=(cdte.data[readtracks].addr_type << 4) |
	   (cdte.data[readtracks].control & 0x0f);
      }
#endif
#ifdef CDROMREADTOCENTRY
      for(readtracks=0;readtracks<=ex->disc_info.num_tracks;readtracks++) {
	 if(readtracks==ex->disc_info.num_tracks)	
	   cdte.cdte_track=CDROM_LEADOUT;
	 else
	   cdte.cdte_track=readtracks+1;
	 
	 cdte.cdte_format=CDROM_MSF;
	 if(ioctl(ex->disc_info.cd_desc,CDROMREADTOCENTRY,&cdte) < 0) {
	    printf(_("Error: Failed to read disc contents\n"));
	    
	    return FALSE;
	 }
	 
	 ex->disc_info.track[readtracks].start_pos.mins=cdte.cdte_addr.msf.minute;
	 ex->disc_info.track[readtracks].start_pos.secs=cdte.cdte_addr.msf.second;
	 frame[readtracks]=cdte.cdte_addr.msf.frame;
	 
	 ex->disc_info.track[readtracks].flags=(cdte.cdte_adr << 4) |
	   (cdte.cdte_ctrl & 0x0f);
      }
#endif
      
      for(readtracks=0;readtracks<=ex->disc_info.num_tracks;readtracks++) {
	 ex->disc_info.track[readtracks].start_frame=
	   (ex->disc_info.track[readtracks].start_pos.mins * 60 +
	    ex->disc_info.track[readtracks].start_pos.secs) * 75 + frame[readtracks];
	 
	 if(readtracks>0) {
	    pos=(ex->disc_info.track[readtracks].start_pos.mins * 60 +
		 ex->disc_info.track[readtracks].start_pos.secs) -
	      (ex->disc_info.track[readtracks-1].start_pos.mins * 60 +
	       ex->disc_info.track[readtracks -1].start_pos.secs);
	    
	    /* Compensate for the gap before a data track */
	    if((readtracks<ex->disc_info.num_tracks&&
		ex_cddev_is_data_track(ex, readtracks)&&
		pos>152)) {
	       pos-=152;
	    }
	    
	    ex->disc_info.track[readtracks - 1].length.mins=pos / 60;
	    ex->disc_info.track[readtracks - 1].length.secs=pos % 60;
	    ex->disc_info.track[readtracks - 1].end_frame = ex->disc_info.track[readtracks].start_frame - 1;
	 }
	 	 
      }
      
      ex->disc_info.length.mins=
	ex->disc_info.track[ex->disc_info.num_tracks].start_pos.mins;
      
      ex->disc_info.length.secs=
	ex->disc_info.track[ex->disc_info.num_tracks].start_pos.secs;

      return 1;
}

/* Update a CD status structure... because operating system interfaces vary
 so does this function. */
/* FIXME can we split this function in two? one the check the status of the disc */
/* and one to actually fill the disc_info struct (read the toc) and set the current
 * frame position, etc */
int
ex_cddev_stat(Extrackt *ex,int read_toc)
{
   /* Since every platform does this a little bit differently this gets pretty
    * complicated... */
#ifdef CDIOREADSUBCHANNEL
   struct ioc_read_subchannel cdsc;
   struct cd_sub_channel_info data;
#endif
#ifdef CDIOREADTOCENTRYS
   struct cd_toc_entry toc_buffer[MAX_TRACKS];
   struct ioc_read_toc_entry cdte;
#endif
#ifdef CDROMSUBCHNL
   struct cdrom_subchnl cdsc;
#endif
#ifdef CDROM_READ_SUBCHANNEL
   struct cd_sub_channel sch;
#endif
#ifdef CDROM_DRIVE_STATUS
   int retcode;
#endif
   
   int pos;
   
   if (ex->disc_info.cd_desc < 0) {
      ex_cddev_init(ex->config.cd->cdrom, ex);
   }
   if (ex->disc_info.cd_desc < 0) {
      return FALSE;
   }
   
#ifdef CDROM_DRIVE_STATUS
   retcode=ioctl(ex->disc_info.cd_desc,CDROM_DRIVE_STATUS,CDSL_CURRENT);
   Debug(_("Drive status is %d\n"),retcode);
   if(retcode < 0) {
      Debug(_("Drive doesn't support drive status check (assume CDS_NO_INFO)\n"));
   }
   else if(retcode != CDS_DISC_OK && retcode != CDS_NO_INFO) {
      Debug(_("No disc\n"));
      ex->disc_info.disc_present=FALSE;
      
      return FALSE;
   }
   
#endif
   
#ifdef CDIOREADSUBCHANNEL
   bzero(&cdsc,sizeof(cdsc));
   cdsc.data=&data;
   cdsc.data_len=sizeof(data);
   cdsc.data_format=CD_CURRENT_POSITION;
   cdsc.address_format=CD_MSF_FORMAT;
   
   if(ioctl(ex->disc_info.cd_desc,CDIOCREADSUBCHANNEL,(char *)&cdsc)<0)
#endif
#ifdef CDROM_READ_SUBCHANNEL
     sch.sch_data_format=CDROM_CURRENT_POSITION;
   
   sch.sch_address_format=CDROM_MSF_FORMAT;
   
   if(ioctl(ex->disc_info.cd_desc,CDROM_READ_SUBCHANNEL, &sch)<0)
#endif
#ifdef CDROMSUBCHNL
     cdsc.cdsc_format=CDROM_MSF;
   
   if(ioctl(ex->disc_info.cd_desc,CDROMSUBCHNL,&cdsc)<0)
#endif
     {
	ex->disc_info.disc_present=FALSE;
	
	return FALSE;
     }
   
#ifdef CDROMSUBCHNL
   if(cdsc.cdsc_audiostatus&&
      (cdsc.cdsc_audiostatus<0x11||cdsc.cdsc_audiostatus>0x15)) {
      ex->disc_info.disc_present=FALSE;
      
      return FALSE;
   }
#endif
   
   ex->disc_info.disc_present=TRUE;
   
#ifdef CDIOREADSUBCHANNEL
   
   ex->disc_info.disc_time.mins=data.what.position.absaddr.msf.minute;
   ex->disc_info.disc_time.secs=data.what.position.absaddr.msf.second;   
   ex->disc_info.curr_frame=(data.what.position.absaddr.msf.minute * 60 +
			     data.what.position.absaddr.msf.second) * 75 +
     data.what.position.absaddr.msf.frame;
   
   switch(data.header.audio_status) {
    case CD_AS_AUDIO_INVALID:
      ex->disc_info.disc_mode=CDAUDIO_NOSTATUS;
      break;
    case CD_AS_PLAY_IN_PROGRESS:
      ex->disc_info.disc_mode=CDAUDIO_PLAYING;
      break;
    case CD_AS_PLAY_PAUSED:
      ex->disc_info.disc_mode=CDAUDIO_PAUSED;
      break;
    case CD_AS_PLAY_COMPLETED:
      ex->disc_info.disc_mode=CDAUDIO_COMPLETED;
      break;
    case CD_AS_PLAY_ERROR:
      ex->disc_info.disc_mode=CDAUDIO_NOSTATUS;
      break;
    case CD_AS_NO_STATUS:
      ex->disc_info.disc_mode=CDAUDIO_NOSTATUS;
   }
#endif
#ifdef CDROMSUBCHNL
   ex->disc_info.disc_time.mins=cdsc.cdsc_absaddr.msf.minute;
   ex->disc_info.disc_time.secs=cdsc.cdsc_absaddr.msf.second;
   ex->disc_info.curr_frame=(cdsc.cdsc_absaddr.msf.minute * 60 +
			     cdsc.cdsc_absaddr.msf.second) * 75 +
     cdsc.cdsc_absaddr.msf.frame;
   
   switch(cdsc.cdsc_audiostatus) {
    case CDROM_AUDIO_PLAY:
      ex->disc_info.disc_mode=CDAUDIO_PLAYING;
      break;
    case CDROM_AUDIO_PAUSED:
      ex->disc_info.disc_mode=CDAUDIO_PAUSED;
      break;
    case CDROM_AUDIO_NO_STATUS:
      ex->disc_info.disc_mode=CDAUDIO_NOSTATUS;
      break;
    case CDROM_AUDIO_COMPLETED:
      ex->disc_info.disc_mode=CDAUDIO_COMPLETED;
      break;
   }
#endif   
     
   if(read_toc)
     ex_cddev_toc_read(ex);

   ex->disc_info.curr_track=0;
   
   while(ex->disc_info.curr_track<ex->disc_info.num_tracks &&
	 ex->disc_info.curr_frame>=ex->disc_info.track[ex->disc_info.curr_track].start_frame)
     ex->disc_info.curr_track++;
   
   pos=(ex->disc_info.curr_frame-ex->disc_info.track[ex->disc_info.curr_track-1].start_frame) / 75;
   
   ex->disc_info.track_time.mins=pos/60;
   ex->disc_info.track_time.secs=pos%60;
   
   return TRUE;
}

/* Check if a track is a data track */
int 
ex_cddev_is_data_track(Extrackt *ex, int track)
{
   return(ex->disc_info.track[track].flags & 4);
}

/* Play frames from CD */
int 
ex_cddev_play_frames(Extrackt *ex, int startframe, int endframe)
{
#ifdef CDIOCPLAYMSF
   struct ioc_play_msf cdmsf;
#endif
#ifdef CDROMPLAYMSF
   struct cdrom_msf cdmsf;
#endif
   
   if (ex->disc_info.cd_desc < 0) {
      return FALSE;
   }
   
#ifdef CDIOCPLAYMSF
   cdmsf.start_m=startframe / (60 * 75);
   cdmsf.start_s=(startframe % (60 * 75)) / 75;
   cdmsf.start_f=startframe % 75;
   cdmsf.end_m=endframe / (60 * 75);
   cdmsf.end_s=(endframe % (60 * 75)) / 75;
   cdmsf.end_f=endframe % 75;
#endif
#ifdef CDROMPLAYMSF
   cdmsf.cdmsf_min0=startframe / (60 * 75);
   cdmsf.cdmsf_sec0=(startframe % (60 * 75)) / 75;
   cdmsf.cdmsf_frame0=startframe % 75;
   cdmsf.cdmsf_min1=endframe / (60 * 75);
   cdmsf.cdmsf_sec1=(endframe % (60 * 75)) / 75;
   cdmsf.cdmsf_frame1=endframe % 75;
#endif
   
#ifdef CDIOCSTART
   if(ioctl(ex->disc_info.cd_desc,CDIOCSTART)<0)
     return FALSE;
#endif
#ifdef CDROMSTART
   if(ioctl(ex->disc_info.cd_desc,CDROMSTART)<0)
     return FALSE;
#endif
#ifdef CDIOCPLAYMSF
   if(ioctl(ex->disc_info.cd_desc,CDIOCPLAYMSF,(char *)&cdmsf)<0)
     return FALSE;
#endif
#ifdef CDROMPLAYMSF
   if(ioctl(ex->disc_info.cd_desc,CDROMPLAYMSF,&cdmsf)<0)
     return FALSE;
#endif
   
   return TRUE;
}

/* Play starttrack at position pos to endtrack */
int
ex_cddev_play_track_pos(Extrackt *ex, int starttrack, int endtrack, int startpos)
{
  if (ex->disc_info.cd_desc < 0) {
    return FALSE;
  }

  return ex_cddev_play_frames(ex, 
			      ex->disc_info.track[starttrack-1].start_frame +
			      startpos * 75,
			      endtrack>=ex->disc_info.num_tracks ?
			      (ex->disc_info.length.mins * 60 +
			       ex->disc_info.length.secs) * 75 :
			      ex->disc_info.track[endtrack].start_frame - 1);
}

/* Play starttrack to endtrack */
int 
ex_cddev_play_track(Extrackt *ex, int starttrack, int endtrack)
{
  return ex_cddev_play_track_pos(ex, starttrack, endtrack, 0);
}

/* Advance (fastfwd) */
int 
ex_cddev_advance(Extrackt *ex, Ex_Disc_Time *time)
{
  if (ex->disc_info.cd_desc < 0) {
    return FALSE;
  }

  ex->disc_info.track_time.mins += time->mins;
  ex->disc_info.track_time.secs += time->secs;

  if(ex->disc_info.track_time.secs > 60) {
    ex->disc_info.track_time.secs -= 60;
    ex->disc_info.track_time.mins++;
  }

  if(ex->disc_info.track_time.secs < 0) {
    ex->disc_info.track_time.secs = 60 + ex->disc_info.track_time.secs;
    ex->disc_info.track_time.mins--;
  }
  
  /*  If we skip back past the beginning of a track, go to the end of
      the last track - DCV */
  if(ex->disc_info.track_time.mins < 0) {
    ex->disc_info.curr_track--;
    
    /*  Tried to skip past first track so go to the beginning  */
    if(ex->disc_info.curr_track == 0) {
      ex->disc_info.curr_track = 1;
      return ex_cddev_play_track(ex,ex->disc_info.curr_track,ex->disc_info.curr_track);
    }
    
    /*  Go to the end of the last track  */
    ex->disc_info.track_time.mins=ex->disc_info.track[(ex->disc_info.curr_track)-1].
      length.mins;
    ex->disc_info.track_time.secs=ex->disc_info.track[(ex->disc_info.curr_track)-1].
      length.secs;

    /*  Try again  */
    return ex_cddev_advance(ex, time);
  }
   
  if((ex->disc_info.track_time.mins ==
      ex->disc_info.track[ex->disc_info.curr_track].start_pos.mins &&
      ex->disc_info.track_time.secs >
      ex->disc_info.track[ex->disc_info.curr_track].start_pos.secs)
     || ex->disc_info.track_time.mins>
     ex->disc_info.track[ex->disc_info.curr_track].start_pos.mins) {
    ex->disc_info.curr_track++;

    if(ex->disc_info.curr_track>ex->disc_info.num_tracks)
      ex->disc_info.curr_track=ex->disc_info.num_tracks;
      
    return ex_cddev_play_track(ex,ex->disc_info.curr_track,ex->disc_info.curr_track);
  }
   
  return ex_cddev_play_track_pos(ex,ex->disc_info.curr_track,ex->disc_info.curr_track,
			ex->disc_info.track_time.mins * 60 +
			ex->disc_info.track_time.secs);
}

/* Stop the CD, if it is playing */
int 
ex_cddev_stop(Extrackt *ex)
{
   if (ex->disc_info.cd_desc < 0) {
      return FALSE;
   }
   
#ifdef CDIOCSTOP
   if(ioctl(ex->disc_info.cd_desc,CDIOCSTOP)<0)
     return FALSE;
#endif
#ifdef CDROMSTOP
   if(ioctl(ex->disc_info.cd_desc,CDROMSTOP)<0)
     return FALSE;
#endif
   
   return TRUE;
}

/* Pause the CD */
int
ex_cddev_pause(Extrackt *ex)
{
   if (ex->disc_info.cd_desc < 0) {
      return FALSE;
   }
   
#ifdef CDIOCPAUSE
   if(ioctl(ex->disc_info.cd_desc,CDIOCPAUSE)<0)
     return FALSE;
#endif
#ifdef CDROMPAUSE
   if(ioctl(ex->disc_info.cd_desc,CDROMPAUSE)<0)
     return FALSE;
#endif
   
   return TRUE;
}

/* Resume playing */
int 
ex_cddev_resume(Extrackt *ex)
{
   if (ex->disc_info.cd_desc < 0) {
      return FALSE;
   }

#ifdef CDIOCRESUME
   if(ioctl(ex->disc_info.cd_desc,CDIOCRESUME)<0)
     return FALSE;
#endif
#ifdef CDROMRESUME
   if(ioctl(ex->disc_info.cd_desc,CDROMRESUME)<0)
     return FALSE;
#endif
   
   return TRUE;
}

/* Check the tray status */
int 
ex_cddec_tray_is_open(Extrackt *ex)
{
#ifdef CDROM_DRIVE_STATUS
   int status;
#endif
   
   if (ex->disc_info.cd_desc < 0) {
      return FALSE;
   }
   
#ifdef CDROM_DRIVE_STATUS
   status=ioctl(ex->disc_info.cd_desc,CDROM_DRIVE_STATUS,CDSL_CURRENT);
   Debug(_("Drive status is %d\n"), status);
   
   if(status < 0) {
      Debug(_("Drive doesn't support drive status check\n"));
      return FALSE;
   }
   
   return status==CDS_TRAY_OPEN;
#endif
   
   return FALSE;
}

/* Eject the tray */
int 
ex_cddev_tray_eject(Extrackt *ex)
{  
   if (ex->disc_info.cd_desc < 0) {
      return FALSE;
   }

#ifdef CDIOCEJECT
   /*  always unlock door before an eject in case something else locked it  */
# if defined(CDROM_LOCKDOOR)
   if(ioctl(ex->disc_info.cd_desc,CDROM_LOCKDOOR,0)<0)
     printf(_("Unlock failed: %d"), errno);
# endif
# ifdef CDIOCALLOW
   if(ioctl(ex->disc_info.cd_desc,CDIOCALLOW)<0)
     printf(_("Unlock failed: %d"),errno);
# endif
   
   if(ioctl(ex->disc_info.cd_desc,CDIOCEJECT)<0) {
      printf(_("CDIOCEJECT"));
      return FALSE;
   }
#endif
#ifdef CDROMEJECT
   if(ioctl(ex->disc_info.cd_desc,CDROMEJECT)<0)
     return FALSE;
#endif
   
   return TRUE;
}

/* Close the tray */

int
ex_cddev_tray_close(Extrackt *ex)
{
   if (ex->disc_info.cd_desc < 0) {
      return FALSE;
   }
   
#ifdef CDIOCCLOSE
   if(ioctl(ex->disc_info.cd_desc,CDIOCCLOSE)<0)
     return FALSE;
#endif
#ifdef CDROMCLOSETRAY
   if(ioctl(ex->disc_info.cd_desc,CDROMCLOSETRAY)<0)
     return FALSE;
#endif
   
   return TRUE;
}

int 
ex_cddev_volume_get(Extrackt *ex, Ex_Disc_Volume *vol)
{
#ifdef CDIOCGETVOL
   struct ioc_vol volume;
#endif
#ifdef CDROMVOLREAD
   struct cdrom_volctrl volume;
#endif
   
   if (ex->disc_info.cd_desc < 0) {
      return FALSE;
   }
   
#ifdef CDIOCGETVOL
   if(ioctl(ex->disc_info.cd_desc,CDIOCGETVOL,&volume)<0)
     return FALSE;
   
   vol->vol_front.left=volume.vol[0];
   vol->vol_front.right=volume.vol[1];
   vol->vol_back.left=volume.vol[2];
   vol->vol_back.right=volume.vol[3];
#endif
#ifdef CDROMVOLREAD
   if(ioctl(ex->disc_info.cd_desc,CDROMVOLREAD, &volume)<0)
     return FALSE;
   
   vol->vol_front.left=volume.channel0;
   vol->vol_front.right=volume.channel1;
   vol->vol_back.left=volume.channel2;
   vol->vol_back.right=volume.channel3;
#endif
   
   return TRUE;
}

int 
ex_cddev_volume_set(Extrackt *ex, Ex_Disc_Volume *vol)
{
#ifdef CDIOCSETVOL
   struct ioc_vol volume;
#endif
#ifdef CDROMVOLCTRL
   struct cdrom_volctrl volume;
#endif
   
   if (ex->disc_info.cd_desc < 0) {
      return FALSE;
   }
   
   if(vol->vol_front.left > 255 || vol->vol_front.left < 0 ||
      vol->vol_front.right > 255 || vol->vol_front.right < 0 ||
      vol->vol_back.left > 255 || vol->vol_back.left < 0 ||
      vol->vol_back.right > 255 || vol->vol_back.right < 0)
     return -1;
   
#ifdef CDIOCSETVOL
   volume.vol[0]=vol->vol_front.left;
   volume.vol[1]=vol->vol_front.right;
   volume.vol[2]=vol->vol_back.left;
   volume.vol[3]=vol->vol_back.right;
   
   if(ioctl(ex->disc_info.cd_desc,CDIOCSETVOL,&volume)<0)
     return FALSE;
#endif
#ifdef CDROMVOLCTRL
   volume.channel0=vol->vol_front.left;
   volume.channel1=vol->vol_front.right;
   volume.channel2=vol->vol_back.left;
   volume.channel3=vol->vol_back.right;
   
   if(ioctl(ex->disc_info.cd_desc,CDROMVOLCTRL,&volume)<0)
     return FALSE;
#endif
   
   return TRUE;
}

/* CD Changer routines */

/* Choose a particular disc from the CD changer */
int 
ex_cddev_disc_select(Extrackt *ex, int disc_num)
{
   if (ex->disc_info.cd_desc < 0) {
      return FALSE;
   }
   
#ifdef CDROM_SELECT_DISC
   if(ioctl(ex->disc_info.cd_desc,CDROM_SELECT_DISC,disc_num)<0)
     return FALSE;
   
   return TRUE;
#else
   errno = ENOSYS;
   
   return FALSE;
#endif
}

/* Identify how many CD-ROMs the changer can hold */
int 
ex_cddev_changer_slots_get(Extrackt *ex)
{
#ifdef CDROM_CHANGER_NSLOTS
   int slots;
#endif
   
   if (ex->disc_info.cd_desc < 0) {
      return FALSE;
   }
   
#ifdef CDROM_CHANGER_NSLOTS
   if((slots=ioctl(ex->disc_info.cd_desc, CDROM_CHANGER_NSLOTS))<0)
     slots=1;
   
   if(slots==0)
     return 1;
   
   return slots;
#else
   return 1;
#endif
}

int
ex_cddev_track_size(Ex_Track_Info *track)
{
   int start_sector;
   int end_sector;
   int size;

   start_sector = 0;
   end_sector = track->end_frame - track->start_frame;
   size = 44+(end_sector - start_sector + 1)*2352;
   printf("first frame %d num frames %d\n", track->start_frame, track->end_frame);
   printf("the file will have this size %d\n", size);
   
   return size;
}
