/*
 * Taken mainly from xmp, (C) 1996-98 Claudio Matsuoka and Hipolito Carraro Jr
 *
 * Completely untested.. so fix it
 */

#ifdef HAVE_SYS_AUDIOIO_H
#include <sys/audioio.h>
#elif defined(HAVE_SYS_AUDIO_IO_H)
#include <sys/audio.io.h>
#elif defined(HAVE_SUN_AUDIOIO_H)
#include <sun/audioio.h>
#endif
#include <errno.h>


#define ARCH_esd_audio_devices
const char *esd_audio_devices()
{
    return "speaker, lineout, headphone";
}


#define ARCH_esd_audio_open
int esd_audio_open()
{
    const char *const default_device = "/dev/audio";
    int afd = -1;
    int mode = O_WRONLY;
    audio_device_t adev;
    const char *device;
    
    if ((device = getenv("AUDIODEV")) == NULL)
      device = default_device;
    
    if ((esd_audio_format & ESD_MASK_FUNC) == ESD_RECORD) {
	fprintf(stderr, "No idea how to record audio on solaris, FIXME\n");
	esd_audio_fd = -1;
	return -1;
    }
    
    if ((afd = open(device, mode)) == -1) {
       if(errno != EACCES && errno != ENOENT)
           perror(device);
       esd_audio_fd = -1;
       return -2;
    }

    if (ioctl(afd, AUDIO_GETDEV, &adev) == -1) {
      perror(device);
      close(afd);
      esd_audio_fd = -1;
      return -1;
    }

    if ( (strcmp(adev.name, "SUNW,CS4231") != 0)
	&& (strcmp(adev.name, "SUNW,sb16")  != 0)
	&& (strcmp(adev.name, "SUNW,dbri") != 0)  ) 
    {
      fprintf(stderr, "No idea how to handle device `%s', FIXME\n", adev.name);
      close(afd);
      esd_audio_fd = -1;
      return -1;
    }

    /* SUNW,CS4231 and compatible drivers */
    {
      int gain = 64;	/* Range: 0 - 255 */
      int port;
      int bsize = 8180;
      audio_info_t ainfo;
      
      if ( esd_audio_device == NULL )
	  port = AUDIO_SPEAKER;
      else if ( !strcmp( esd_audio_device, "lineout" ) )
	  port = AUDIO_LINE_OUT;
      else if ( !strcmp( esd_audio_device, "speaker" ) )
	  port = AUDIO_SPEAKER;
      else if ( !strcmp( esd_audio_device, "headphone" ) )
	  port = AUDIO_HEADPHONE;
      else {
	  fprintf(stderr, "Unknown output device `%s'\n", esd_audio_device);
	  close(afd);
	  esd_audio_fd = -1;
	  return -1;
      }

      AUDIO_INITINFO(&ainfo);
      
      ainfo.play.sample_rate = esd_audio_rate;
      
      if ((esd_audio_format & ESD_MASK_CHAN) == ESD_STEREO)
        ainfo.play.channels = 2;
      else
	ainfo.play.channels = 1;
      
      if ((esd_audio_format & ESD_MASK_BITS) == ESD_BITS16)
        ainfo.play.precision = 16;
      else
	ainfo.play.precision = 8;
      
      ainfo.play.encoding = AUDIO_ENCODING_LINEAR;
      ainfo.play.gain = gain;
      ainfo.play.port = port;
      ainfo.play.balance = AUDIO_MID_BALANCE;
      ainfo.play.buffer_size = bsize;
      
      if (ioctl(afd, AUDIO_SETINFO, &ainfo) == -1)
	{
	  perror("AUDIO_SETINFO");
	  close(afd);
	  esd_audio_fd = -1;
	  return -1;
	}
    }
    
    esd_audio_fd = afd;
    return afd;
}	    

