/* Advanced Linux Sounds Architecture support for EsounD
   7-19-98: Nick Lopez( kimo_sabe@usa.net ) - it starts!
*/
#include "esd.h"

/* debugging messages for audio device */
static int driver_trace = 0;

#if defined(DRIVER_NEWALSA)
#  include <sys/asoundlib.h>
#else
#  include <sys/soundlib.h>
#endif

void *handle;

/* so that EsounD can use other cards besides the first */
#ifndef ALSACARD
#  define ALSACARD 0
#endif
#ifndef ALSADEVICE
#  define ALSADEVICE 0
#endif

/* some identifiers changed names */
#ifndef SND_PCM_OPEN_RECORD
#define SND_PCM_OPEN_RECORD SND_PCM_OPEN_CAPTURE
#endif

#ifndef snd_pcm_record_format
#define snd_pcm_record_format snd_pcm_capture_format
#endif

#define ARCH_esd_audio_open
static void
alsa_print_error (int code, int card, int device) {
    if( driver_trace ) { 
	perror( "snd_ctl_open" );

	if( device >= 0 ) {
	    fprintf (stderr, "card %d pcm device %d open failed: %s\n",
	    	     card, device, snd_strerror( code ) );
	}
	else {
	    fprintf( stderr, "card %d open failed: %s\n", 
	  	     card, snd_strerror( code ) );
	}
    }    
}

int esd_audio_open()
{
    snd_pcm_format_t format;
    snd_pcm_playback_params_t params;
    int ret, mode = SND_PCM_OPEN_PLAYBACK;
    int mask, card=ALSACARD, device=ALSADEVICE;
    char buf[256];
    void *ctl_handle;
    struct snd_ctl_hw_info hw_info;
  
    /* if recording, set for full duplex mode */
    if ( (esd_audio_format & ESD_MASK_FUNC) == ESD_RECORD )
        mode = SND_PCM_OPEN_DUPLEX;
  
#if 0 /* single card code, just in case anyone needs it */
    if ( ret = snd_pcm_open( &handle, ALSACARD, ALSADEVICE, mode ) < 0) {
	perror( "snd_pcm_open" );
	fprintf( stderr, "open failed: %s\n", snd_strerror( ret ) );
	esd_audio_close();
	esd_audio_fd = -1;
	return ( -1 );
    }
    
#else /* multiple card code, open the first available.  someone check it? */

    mask = snd_cards_mask();
    if ( !mask ) {
	fprintf( stderr, "audio_alsa: no cards found!" );
	esd_audio_close();
	esd_audio_fd = -1;
	return ( -1 );
    }

    handle = NULL;
    for ( card=0; ( card < SND_CARDS ) && (handle == NULL); card++ ) {
	if ( mask & (1 << card) ) {
	    /* open sound card */
	    ret = snd_ctl_open( &ctl_handle, card );

	    if ( ret < 0 ) {
		alsa_print_error( ret, card, -1 );
		continue;
	    }
	    
	    if ( driver_trace ) {
		fprintf( stderr, "opened alsa card %d\n", card );
	    }
	   
	    /* get info on sound card */
	    ret = snd_ctl_hw_info( ctl_handle, &hw_info );
	    if ( ret < 0 ) {
		alsa_print_error( ret, card, -1 );
		continue;
	    }
	    ret = snd_ctl_close( ctl_handle );
	    if ( ret < 0 ) {
		alsa_print_error( ret, card, -1 );
		continue;
	    }

	    /* search for available pcm device on card */
	    for ( device=0; (device < hw_info.pcmdevs) && (handle == NULL);
	    	 device++ ) {
		ret = snd_pcm_open( &handle, card, device, mode );
		if ( ret < 0 ) {
		    alsa_print_error( ret, card, device );
		    handle = NULL;
		    continue;
		}
	    }
	    device--;
	    
	    if ( (handle != NULL) && driver_trace ) {
	       fprintf( stderr, "opened alsa card %d pcm device %d\n",
		      card, device );
	    }
	}
    }
    card--;
  
    if ( handle == NULL ) {
	fprintf( stderr, "Couldn't open any alsa card! Last card tried was %d\n", 
		 card );
	fprintf( stderr, "Error opening card %d: %s\n", 
		 card, snd_strerror( ret ) );  
	
	esd_audio_close();
	esd_audio_fd = -1;
	return ( -1 );
    }
    
#endif

    /* set the sound driver audio format for playback */
    format.format = ( (esd_audio_format & ESD_MASK_BITS) == ESD_BITS16 )
	? /* 16 bit */ SND_PCM_SFMT_S16_LE : /* 8 bit */ SND_PCM_SFMT_U8;
    format.rate = esd_audio_rate;
    format.channels = ( ( esd_audio_format & ESD_MASK_CHAN) == ESD_STEREO ) 
	? 2 : 1;

    if( mode == SND_PCM_OPEN_DUPLEX || mode == SND_PCM_OPEN_PLAYBACK ) {
        if ( ( ret = snd_pcm_playback_format( handle, &format ) ) < 0 ) {
	    fprintf( stderr, "set playback format failed: %s\n", snd_strerror( ret ) );
	    esd_audio_close();
	    esd_audio_fd = -1;
	    return ( -1 );
	}
    }
    if( mode == SND_PCM_OPEN_DUPLEX || mode == SND_PCM_OPEN_RECORD ) {
        if ( ( ret = snd_pcm_record_format( handle, &format ) ) < 0 ) {
	    fprintf( stderr, "set record format failed: %s\n", snd_strerror( ret ) );
	    esd_audio_close();
	    esd_audio_fd = -1;
	    return ( -1 );
	}
    }    

    params.fragment_size = 4*1024;
    params.fragments_max = 2;
    params.fragments_room = 1;
    ret = snd_pcm_playback_params( handle, &params );
    if ( ret ) {
	printf( "error: %s: in snd_pcm_playback_params\n", snd_strerror(ret) );
    }

/* shouldn't use non-blocking mode, because you have to sit in a loop rewriting
   data until success (eating cpu time in the process).  This wasn't being done,
   and didn't work on my machine.  Or you could use select(man page 2), I guess.
*/
#if 0
    ret = snd_pcm_block_mode( handle, 1 );
    if ( ret )
	printf( "error: %s: in snd_pcm_block_mode\n", snd_strerror(ret));
#endif

    if ( format.rate != esd_audio_rate || format.channels != 2 
	 || format.format != SND_PCM_SFMT_S16_LE )
	printf("set format didn't work.");

    /* no descriptor for ALSAlib */
    return ( esd_audio_fd = snd_pcm_file_descriptor( handle ) ); 
}

#define ARCH_esd_audio_close
void esd_audio_close()
{
    snd_pcm_close( handle );
}

#define ARCH_esd_audio_pause
void esd_audio_pause()
{
    /* apparently this gets rid of pending data, which isn't the effect
       we're going for, namely, play the data in the buffers and stop */
    /* snd_pcm_drain_playback( handle ); */
}

#define ARCH_esd_audio_read
int esd_audio_read( void *buffer, int buf_size )
{
    return (snd_pcm_read( handle, buffer, buf_size ));
}

int writes;
#define ARCH_esd_audio_write
int esd_audio_write( void *buffer, int buf_size )
{
    int i=0;
    i = snd_pcm_write( handle, buffer, buf_size);
    writes += i;
    return (i);
}

#define ARCH_esd_audio_flush
void esd_audio_flush()
{
    fsync( esd_audio_fd );
    /*snd_pcm_flush_playback( handle );*/
}
