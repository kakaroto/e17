
#include "config.h"
#include "esd.h"
#include "genrand.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/wait.h>

#include <sys/un.h>

/*******************************************************************/
/* prototypes */
int esd_set_socket_buffers( int sock, int src_format, 
			    int src_rate, int base_rate );
static void dummy_signal(int signum);

/* dummy handler */
static void dummy_signal(int signum) {
    signal( signum, dummy_signal);
    return;
}

/* from esd_config.c */
extern char esd_spawn_options[];
extern int esd_no_spawn;
extern int esd_spawn_wait_ms;
void esd_config_read(void);

/*******************************************************************/
/* alternate implementations */
#ifndef HAVE_INET_ATON
int inet_aton(const char *cp, struct in_addr *inp)
{
    union {
	unsigned int n;
	char parts[4];
    } u;
    int a=0,b=0,c=0,d=0, i;

    i = sscanf(cp, "%d.%d.%d.%d%*s", &a, &b, &c, &d);

    if(i != 4)
	return 0;

    u.parts[0] = a;
    u.parts[1] = b;
    u.parts[2] = c;
    u.parts[3] = d;

    inp->s_addr = u.n;

    return 1;
}
#endif

/**
 * esd_set_socket_buffers: set buffer lengths on a socket to optimal.
 * @sock: ESD socket
 * @src_format: data format
 * @src_rate: sample rate for this stream
 * @base_rate: sample rate that server is running at.
 * 
 * Sets send and receive buffer lengths to optimal length for audio data
 * transfer.
 *
 * Return Value: Size that buffers were set to.
 */
int esd_set_socket_buffers( int sock, int src_format, 
			    int src_rate, int base_rate )
{
    int buf_size = ESD_BUF_SIZE;

    if ( src_rate > 0 ) buf_size = ( buf_size * base_rate ) / src_rate;
    if ( ( src_format & ESD_MASK_BITS ) == ESD_BITS16 )
	buf_size *= 2;
    if ( ! ( ( src_format & ESD_MASK_CHAN ) == ESD_MONO ) )
	buf_size *= 2;

    setsockopt( sock, SOL_SOCKET, SO_SNDBUF, &buf_size, sizeof( buf_size ) );
    setsockopt( sock, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof( buf_size ) );
    return buf_size;
}

/**
 * esd_get_latency: get stream latency
 * @esd: ESD socket
 *
 * Get the stream latency to esound (latency is number of samples
 * at 44.1khz stereo 16 bit - you'll have to adjust if oyur input
 * sampling rate is less (in bytes per second))
 *
 * Return Value: Latency, in number of samples at 44.1khz stereo 16 bit.
 */
int esd_get_latency(int esd)
{
    int lag = 0;
    int proto = ESD_PROTO_LATENCY;
    void (*phandler)(int);

/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
    /* send the necessary information */
    if ( write( esd, &proto, sizeof(proto) ) != sizeof(proto) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    /* get the latency back from the server */
    if ( read( esd, &lag, sizeof(lag) ) != sizeof(lag) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound getting latency\n" );
    */

    /* return the sample id to the client */
    signal( SIGPIPE, phandler ); 
  
    lag += ESD_BUF_SIZE * 2;
  
    return lag;
}

/**
 * esd_send_auth: send authorization to esd
 * @sock: ESD socket
 * 
 * Send the authorization cookie, creating a new one if needed.
 *
 * Return Value: -1 on error, 0 if authorization was refused by server,
 * 1 if authorization was accepted.
 **/
int esd_send_auth( int sock )
{
    int auth_fd = -1, i = 0;
    int endian = ESD_ENDIAN_KEY;
    int reply;
    char *auth_filename = 0, auth_key[ESD_KEY_LEN];
    char *home = NULL;
    char tumbler = '\0';
    int namelen, retval;
    void (*phandler)(int);
  
/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
  
    /* assemble the authorization filename */
    home = getenv( "HOME" );
    if ( !home ) {
	fprintf( stderr, "HOME environment variable not set?\n" );
        signal( SIGPIPE, phandler ); 
	return -1;
    }

    namelen = strlen(home) + sizeof("/.esd_auth");
    if ((auth_filename = malloc(namelen + 1)) == 0) {
	fprintf( stderr, "Memory exhausted\n" );
        signal( SIGPIPE, phandler ); 
	return -1;
    }

    strcpy( auth_filename, home );
    strcat( auth_filename, "/.esd_auth" );

    retval = 0;
    /* open the authorization file */
    if ( -1 == (auth_fd = open( auth_filename, O_RDONLY ) ) ) {
        unsigned char randbuf[ESD_KEY_LEN];

	/* it doesn't exist? create one */
	auth_fd = open( auth_filename, O_RDWR | O_CREAT | O_EXCL,
			S_IRUSR | S_IWUSR );

	if ( -1 == auth_fd ) {
	    /* coun't even create it?  bail */
	    perror( auth_filename );
	    goto exit_fn;
	}

	esound_genrand(auth_key, ESD_KEY_LEN);
	write( auth_fd, randbuf, ESD_KEY_LEN);
    } else
      /* read the key from the authorization file */
      if ( ESD_KEY_LEN != read( auth_fd, auth_key, ESD_KEY_LEN ) )
	goto exit_fd;

    /* send the key to the server */
    if ( ESD_KEY_LEN != write( sock, auth_key, ESD_KEY_LEN ) )
	/* send key failed */
	goto exit_fd;

    /* send the key to the server */
    if ( sizeof(endian) != write( sock, &endian, sizeof(endian) ) )
	/* send key failed */
	goto exit_fd;

    /* read auth reply. esd will reply 1 as an int for yes and 0 for no */
    /* then close the connection */
    if ( sizeof(reply) != read( sock, &reply, sizeof(reply) ) ) {
	/* read ok failed */
	retval = 0;
	goto exit_fd;
    }
    /* we got a reply and it's no - so esd will close the socket now */
    /* on us anyway... time to return invalid auth... */
    if (reply == 0) {
	/* auth failed */
	retval = 0;
	goto exit_fd;
    }
  
  
    /* we've run the gauntlet, everything's ok, proceed as usual */
    /* fsync( sock ); */
    retval = 1;

 exit_fd:
    close( auth_fd );
 exit_fn:
    free( auth_filename );
    signal( SIGPIPE, phandler ); 
    return retval;
}

/**
 * esd_lock: disable foreign clients
 * @esd: ESD socket
 * 
 * Locks the ESD on the end of the specified socket, so that it will not
 * accept connections from untrusted clients - eg, clients which do not
 * present the appropriate key.
 *
 * Counterpart to esd_unlock().
 *
 * Return Value: -1 on error, 0 if failed, otherwise success.
 */
int esd_lock( int esd ) {
    int proto = ESD_PROTO_LOCK;
    int ok = 0;
    void (*phandler)(int);
  
/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound locking\n" );
    */

    write( esd, &proto, sizeof(proto) );
    esd_send_auth( esd );

    if ( read( esd, &ok, sizeof(ok) ) != sizeof(ok) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    signal( SIGPIPE, phandler ); 
    return ok;
}

/**
 * esd_unlock: disable foreign clients
 * @esd: ESD socket
 * 
 * Unlocks the ESD on the end of the specified socket, so that it will
 * accept connections from untrusted clients - eg, clients which do not
 * present the appropriate key.
 *
 * Counterpart to esd_lock().
 *
 * Return Value: -1 on error, 0 if failed, otherwise success.
 */
int esd_unlock( int esd ){
    int proto = ESD_PROTO_UNLOCK;
    int ok = 0;
    void (*phandler)(int);
  
/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound unlocking\n" );
    */

    write( esd, &proto, sizeof(proto) );
    esd_send_auth( esd );

    if ( read( esd, &ok, sizeof(ok) ) != sizeof(ok) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    signal( SIGPIPE, phandler ); 
    return ok;
}

/**
 * esd_standby: release audio device
 * @esd: ESD socket
 * 
 * Causes the ESD on the end of the specified socket to stop playing sounds
 * and release its connection to the audio device so that other processes
 * may use it.
 *
 * Counterpart to esd_resume().
 *
 * Return Value: -1 on error, 0 if failed, otherwise success.
 */
int esd_standby( int esd )
{
    int proto = ESD_PROTO_STANDBY;
    int ok = 0;
    void (*phandler)(int);
  
/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound standing by\n" );
    */

    write( esd, &proto, sizeof(proto) );
    esd_send_auth( esd );

    if ( read( esd, &ok, sizeof(ok) ) != sizeof(ok) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    signal( SIGPIPE, phandler ); 
    return ok;
}

/**
 * esd_resume: reclaim audio device
 * @esd: ESD socket
 * 
 * Causes the ESD on the end of the specified socket to attempt to reconnect
 * to the audio device and start playing sounds again.
 *
 * Counterpart to esd_standby().
 *
 * Return Value: -1 on error, 0 if failed, otherwise success.
 */
int esd_resume( int esd )
{
    int proto = ESD_PROTO_RESUME;
    int ok = 0;
    void (*phandler)(int);
  
/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound resuming\n" );
    */

    write( esd, &proto, sizeof(proto) );
    esd_send_auth( esd );

    if ( read( esd, &ok, sizeof(ok) ) != sizeof(ok) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    signal( SIGPIPE, phandler ); 
    return ok;
}

/**
 * esd_connect_tcpip: make a TCPIP connection to ESD
 * @host: Specifies hostname and port to connect to as "hostname:port"
 * Both parts are optional, the default being to connect to localhost on
 * ESD_DEFAULT_PORT.  This default is used if host is NULL.
 *
 * Attempts to make a connection to ESD using TCPIP.
 * Similar to esd_connect_unix().
 *
 * Return Value: -1 on error, else a socket number connected to ESD.
 */
static int
esd_connect_tcpip(const char *host)
{
    const char *espeaker = NULL;
    struct hostent *he;
    struct sockaddr_in socket_addr;
    int socket_out = -1;
    int curstate = 1;
    char default_host[] = "0.0.0.0";
    char connect_host[64];
    int port = ESD_DEFAULT_PORT;
    unsigned int host_div = 0;
  
    /* see if we have a remote speaker to play to */
    espeaker = host;
    if ( espeaker && *espeaker ) {
	strncpy(connect_host, espeaker, sizeof(connect_host));

	/* split the espeaker host into host:port */
	host_div = strcspn( connect_host, ":" );
	if(host_div > 0 && host_div < strlen(espeaker)) {
	    connect_host[ host_div ] = '\0';
	}
	else if ( host_div == 0)
		strcpy( connect_host, default_host );

	connect_host[sizeof(connect_host) - 1] = '\0';
    
	/* Resolving the host name */
	if ( ( he = gethostbyname( connect_host ) ) == NULL ) {
	    fprintf( stderr, "Can\'t resolve host name \"%s\"!\n", 
		     connect_host);
	    return(-1);
	}
	memcpy( (struct in_addr *) &socket_addr.sin_addr, he->h_addr,
		sizeof( struct in_addr ) );
    
	/* get port */
	if ( host_div < strlen( espeaker ) )
	    port = atoi( espeaker + host_div + 1 );
	if ( !port ) 
	    port = ESD_DEFAULT_PORT;
	/* printf( "(remote) host is %s : %d\n", connect_host, port ); */
    } else if( !inet_aton( default_host, &socket_addr.sin_addr ) ) {
	fprintf( stderr, "couldn't convert %s to inet address\n", 
		 default_host );
	return -1;
    }
  
    /* create the socket, and set for non-blocking */
    socket_out = socket( AF_INET, SOCK_STREAM, 0 );
    if ( socket_out < 0 ) 
    {
	fprintf(stderr,"Unable to create TCP socket\n");
	goto error_out;
    }
  
    /* this was borrowed blindly from the Tcl socket stuff */
    if ( fcntl( socket_out, F_SETFD, FD_CLOEXEC ) < 0 )
    {
	fprintf(stderr,"Unable to set socket to non-blocking\n");
	goto error_out;
    }

    if ( setsockopt( socket_out, SOL_SOCKET, SO_REUSEADDR,
		     &curstate, sizeof(curstate) ) < 0 ) 
    {
	fprintf(stderr,"Unable to set for a fresh socket\n");
	goto error_out;
    }
  
    /* set the connect information */
    socket_addr.sin_family = AF_INET;
    socket_addr.sin_port = htons( port );
  
    if ( connect( socket_out,
		  (struct sockaddr *) &socket_addr,
		  sizeof(struct sockaddr_in) ) < 0 )
	goto error_out;

    return socket_out;

 error_out:
    if( socket_out >= 0 )
	close( socket_out );
    return -1;
}

/**
 * esd_connect_unix: make a local UNIX socket connection to ESD
 * @host: Host to connect to - ignored, since UNIX sockets
 * - FIXME - tidy up by removing this parameter.
 *
 * Attempts to make a connection to ESD using local UNIX sockets.
 * Similar to esd_connect_tcpip().
 *
 * Return Value: -1 on error, else a socket number connected to ESD.
 */
static int
esd_connect_unix(const char *host)
{
    struct sockaddr_un socket_unix;
    int socket_out = -1;
    int curstate = 1;
  
    /* create the socket, and set for non-blocking */
    socket_out = socket( AF_UNIX, SOCK_STREAM, 0 );
    if ( socket_out < 0 ) 
    {
	fprintf(stderr,"Unable to create socket\n");
	goto error_out;
    }
  
    /* this was borrowed blindly from the Tcl socket stuff */
    if ( fcntl( socket_out, F_SETFD, FD_CLOEXEC ) < 0 )
    {
	fprintf(stderr,"Unable to set socket to close-on-exec\n");
	goto error_out;
    }
    if ( setsockopt( socket_out, SOL_SOCKET, SO_REUSEADDR,
		     &curstate, sizeof(curstate) ) < 0 ) 
    {
	fprintf(stderr,"Unable to set for a fresh socket\n");
	goto error_out;
    }
  
    /* set the connect information */
    socket_unix.sun_family = AF_UNIX;
    strncpy(socket_unix.sun_path, ESD_UNIX_SOCKET_NAME, sizeof(socket_unix.sun_path));
  
    if ( connect( socket_out,
		  (struct sockaddr *) &socket_unix, SUN_LEN(&socket_unix) ) < 0 )
	goto error_out;
  
    return socket_out;

 error_out:
    if(socket_out >= 0)
	close(socket_out);
    return -1;
}

static int got_sigusr1 = 0, got_sigalrm = 0;
static void
esd_handle_sig(int signum)
{
    switch(signum) {
    case SIGUSR1:
	got_sigusr1++;
	break;
    case SIGALRM:
	got_sigalrm++;
	break;
    }
}

/**
 * esd_open_sound: open a connection to ESD and get authorization
 * @host: Specifies hostname and port to connect to as "hostname:port"
 * Both parts are optional, the default being to connect to localhost on
 * ESD_DEFAULT_PORT.  This default is used if host is NULL.
 *
 * Attempts to make a connection to ESD on specified host, or the
 * host specified by the $ESPEAKER environment variable if @host is NULL,
 * or localhost if $ESPEAKER not set.
 *
 * Will attempt to connect by UNIX sockets if the host is localhost, and by
 * TCPIP otherwise, or if UNIX sockets fail.
 *
 * If neither of these connection methods succeeds, and we are attempting to
 * contact the localhost, will attempt to spawn a local copy of ESD (unless
 * configured not to in esd.conf), and will then try to connect to that
 * using UNIX sockets.
 *
 * Once a connection is created, attempts to give ESD the neccessary
 * authorisation keys to do things - only if this succeeds will the socket
 * be given to the caller.
 *
 * Return Value: -1 on error, else a socket number connected and authorized
 * to ESD.
 */
#define min(a,b) ( ( (a)<(b) ) ? (a) : (b) )
int esd_open_sound( const char *host )
{
    int connect_count;
    int socket_out = -1;
    int len;
    char use_unix = 0;
    char display_host[ 256 ];
    char *display;

    if ( !host ) host = getenv("ESPEAKER");

    display = getenv( "DISPLAY" );
    if ( !(host && *host) && display ) {
	/* no espeaker specified, but the app should be directed to a
	   remote display, so try routing the default port over there
	   and see if we strike gold */
	len = strcspn( display, ":" );
	if ( len ) {
	    len = min( len, 256 ); 
	    strncpy( display_host, display, len );
	    display_host[ len ] = '\0';
	    host = display_host;
	}
    }

    if ( !(host && *host)) {
	if ( access( ESD_UNIX_SOCKET_NAME, R_OK | W_OK ) == -1 )
	    use_unix = 0;
	else
	    use_unix = 1;
    }
    if ( use_unix )
	socket_out = esd_connect_unix( NULL );
    if ( socket_out >= 0 ) goto finish_connect;

    socket_out = esd_connect_tcpip( host );
    if ( socket_out >= 0 ) goto finish_connect;

    /* Connections failed, period. Since nobody gave us a remote esd
       to use, let's try spawning one. */
    /* ebm - I think this is an Inherently Bad Idea, but will leave it
       alone until I can get a good look at it */
    if(! (host && *host)) {
	int childpid, mypid;
	struct sigaction sa, sa_orig;
	struct sigaction sa_alarm, sa_orig_alarm;

	esd_config_read();

	if (esd_no_spawn) goto finish_connect;

	/* All this hackery so we can stop thrashing around if esd startup fails */
	/* there's something inherently flaky about this, and if
	   there's no audio device, Bad Things Happen */

	mypid = getpid();
	memset(&sa, '\0', sizeof(sa));
	memset(&sa_alarm, '\0', sizeof(sa));
	sa.sa_handler = esd_handle_sig;
	sa_alarm.sa_handler = esd_handle_sig;
	sigaction(SIGUSR1, &sa, &sa_orig);
	alarm(0);
	sigaction(SIGALRM, &sa_alarm, &sa_orig_alarm);

	childpid = fork();
	if(!childpid) {
	    /* child process */
	    if(!fork()) {
		/* child of child process */
		char *cmd;

		setsid();
		cmd = malloc(sizeof("esd ") + esd_spawn_options?strlen(esd_spawn_options):0);

		sprintf(cmd, "esd %s -spawnpid %d", esd_spawn_options?esd_spawn_options:"", mypid);

		execl("/bin/sh", "/bin/sh", "-c", cmd, NULL);
		perror("execl");
		_exit(1);
	    } else
		_exit(0);

	    /* children end here */
	} else {
	    int estat;

	    waitpid(childpid, &estat, 0);
	}

	/* Wait for for spawning to happen.  Time taken is system and load
	 * dependent, so read from config file.
	 */
	for(connect_count = 0; connect_count < esd_spawn_wait_ms; connect_count++) {
	    alarm(10);
	    pause(); /* Until we either get USR1 (esd startup failed), USR2 (esd startup OK), or ALRM (timeout) */
	    alarm(0);

	    if(got_sigusr1) {
		socket_out = esd_connect_unix(host);
		if (socket_out < 0)
		    socket_out = esd_connect_tcpip(host);
		if (socket_out >= 0) break;
	    } else if(got_sigalrm)
		break;
	}

	sigaction(SIGUSR1, &sa_orig, NULL);
	sigaction(SIGALRM, &sa_orig_alarm, NULL);
    }

 finish_connect:
    if (socket_out >= 0
	&& !esd_send_auth (socket_out)) {
	close(socket_out); socket_out = -1;
    }

    return socket_out;
}

/**
 * esd_play_stream: get socket for playing a stream
 * @format: data format for this stream
 * @rate: sample rate for this stream
 * @host: host to connect to, as for esd_open_sound().
 * @name: name by which to identify this stream to ESD.  (Use NULL if you
 * don't care what name you're given - but its generally more useful to give
 * something helpful, such as your process name and id.)
 *
 * Creates a new connection to ESD, using esd_open_sound(), and sets it up
 * for playing a stream of audio data, at sample rate @rate, 
 *
 * Return Value: -1 on error, else an ESD socket number set up so that
 * any data sent to the socket will be played by the ESD.
 */
int esd_play_stream( esd_format_t format, int rate, 
		     const char *host, const char *name )
{
    int sock;
    int proto = ESD_PROTO_STREAM_PLAY;
    char name_buf[ ESD_NAME_MAX ];
    void (*phandler)(int);
      
    /* connect to the EsounD server */
    sock = esd_open_sound( host );
    if ( sock < 0 ) 
	return sock;

    /* prepare the name buffer */
    if ( name )
	strncpy( name_buf, name, ESD_NAME_MAX );
    else
	name_buf[ 0 ] = '\0';

/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
    /* send the audio format information */
    if ( write( sock, &proto, sizeof(proto) ) != sizeof(proto) ) {
	signal( SIGPIPE, phandler );
	return -1;
    }
    if ( write( sock, &format, sizeof(format) ) != sizeof(format) ) {
	signal( SIGPIPE, phandler );
	return -1;
    }
    if( write( sock, &rate, sizeof(rate) ) != sizeof(rate) ) {
	signal( SIGPIPE, phandler );
	return -1;
    }
    if( write( sock, name_buf, ESD_NAME_MAX ) != ESD_NAME_MAX ) {
	signal( SIGPIPE, phandler );
	return -1;
    }

    /* Reduce buffers on sockets to the minimum needed */
    esd_set_socket_buffers( sock, format, rate, 44100 );

    /* flush the socket */
    /* fsync( sock ); */
    
    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound playing stream\n" );
    */

    signal( SIGPIPE, phandler );
    return sock;
}

/**
 * esd_play_stream_fallback: as esd_play_stream() but connect directly if no ESD
 * @format: data format for this stream
 * @rate: sample rate for this stream
 * @host: host to connect to, as for esd_open_sound().
 * @name: name by which to identify this stream to ESD.
 *
 * Attempts to create a connection to an ESD, using esd_play_stream(), and if
 * this fails falls back to trying to contact the soundcard directly.
 * (This will not work unless the soundcard is local.)
 *
 * Return Value: -1 on error, else a socket number set up so that
 * any data sent to the socket will be played, either by an ESD or by an.
 */
int esd_play_stream_fallback( esd_format_t format, int rate, 
			      const char *host, const char *name )
{
    int socket_out;

    /* try to open a connection to the server */
    if(!host) host = getenv("ESPEAKER");
    socket_out = esd_play_stream( format, rate, host, name );
    if ( socket_out >= 0 ) 
	return socket_out;

    /* if host is set, this is an error, bail out */
    if ( host )
	return -1;

    /* go for /dev/dsp */
    esd_audio_format = format;
    esd_audio_rate = rate;
    socket_out = esd_audio_open();

    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound playing stream fallback\n" );
    */

    /* we either got it, or we didn't */
    return socket_out;
}

/**
 * esd_monitor_stream: get socket for monitoring an ESD
 * @format: data format for this stream
 * @rate: sample rate for this stream
 * @host: host to connect to, as for esd_open_sound().
 * @name: name by which identify this stream to ESD.
 *
 * Creates a new connection to ESD, using esd_open_sound(), and sets it up
 * for monitoring the output from the ESD.
 *
 * Return Value: -1 on error, else an ESD socket number set up so that
 * any data played by the ESD will be sent to the socket.
 */
int esd_monitor_stream( esd_format_t format, int rate, 
			const char *host, const char *name )
{
    int sock;
    int proto = ESD_PROTO_STREAM_MON;
    char name_buf[ ESD_NAME_MAX ];
    void (*phandler)(int);

    /* connect to the EsounD server */
    sock = esd_open_sound( host );
    if ( sock < 0 ) 
	return sock;
    
    /* prepare the name buffer */
    if ( name )
	strncpy( name_buf, name, ESD_NAME_MAX );
    else
	name_buf[ 0 ] = '\0';

/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
    /* send the audio format information */
    if ( write( sock, &proto, sizeof(proto) ) != sizeof(proto) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    if ( write( sock, &format, sizeof(format) ) != sizeof(format) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    if( write( sock, &rate, sizeof(rate) ) != sizeof(rate) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    if( write( sock, name_buf, ESD_NAME_MAX ) != ESD_NAME_MAX ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    /* Reduce buffers on sockets to the minimum needed */
    esd_set_socket_buffers( sock, format, rate, 44100 );

    /* flush the socket */
    /* fsync( sock ); */
    
    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound monitoring stream\n" );
    */

    signal( SIGPIPE, phandler ); 
    return sock;
}

/**
 * esd_filter_stream: get socket for filtering sound produced by an ESD
 * @format: data format for this stream
 * @rate: sample rate for this stream
 * @host: host to connect to, as for esd_open_sound().
 * @name: name by which identify this stream to ESD.
 *
 * Creates a new connection to ESD, using esd_open_sound(), and sets it up
 * for filtering the output from the ESD.
 * 
 * Reading from the stream will give a block of audio data, which is the
 * mixed output of the ESD formatted as specified by the function parameters.
 * The filter is free to process this data as it likes, but must then write
 * an indentically sized block of data back to the stream.  The data so
 * returned is played by the ESD (possibly after applying more filters to
 * it.)
 *
 * The new filter will be placed at the head of the list of filters  ie, it
 * will receive data for processing first, and the next filter will receive
 * the resultant processed data.
 *
 * Return Value: -1 on error, else an ESD socket number set up so that
 * any data played by the ESD will be sent to the socket.
 */
int esd_filter_stream( esd_format_t format, int rate, 
		       const char *host, const char *name )
{
    int sock;
    int proto = ESD_PROTO_STREAM_FILT;
    char name_buf[ ESD_NAME_MAX ];
    void (*phandler)(int);

    /* connect to the EsounD server */
    sock = esd_open_sound( host );
    if ( sock < 0 ) 
	return sock;
    
    /* prepare the name buffer */
    if ( name )
	strncpy( name_buf, name, ESD_NAME_MAX );
    else
	name_buf[ 0 ] = '\0';

/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
    /* send the audio format information */
    if ( write( sock, &proto, sizeof(proto) ) != sizeof(proto) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    if ( write( sock, &format, sizeof(format) ) != sizeof(format) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    if( write( sock, &rate, sizeof(rate) ) != sizeof(rate) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    if( write( sock, name_buf, ESD_NAME_MAX ) != ESD_NAME_MAX ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    /* Reduce buffers on sockets to the minimum needed */
    esd_set_socket_buffers( sock, format, rate, 44100 );

    /* flush the socket */
    /* fsync( sock ); */
    
    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound filterng stream\n" );
    */

    signal( SIGPIPE, phandler ); 
    return sock;
}

/**
 * esd_record_stream: get socket for recording via an ESD
 * @format: data format for this stream
 * @rate: sample rate for this stream
 * @host: host to connect to, as for esd_open_sound().
 * @name: name by which identify this stream to ESD.
 *
 * Creates a new connection to ESD, using esd_open_sound(), and sets it up
 * for recording data from the soundcard via the ESD.
 *
 * Return Value: -1 on error, else a socket number to which the external
 * audio data arriving at the appropriate soundcard will be sent.
 */
int esd_record_stream( esd_format_t format, int rate, 
		       const char *host, const char *name )
{
    int sock;
    int proto = ESD_PROTO_STREAM_REC;
    char name_buf[ ESD_NAME_MAX ];
    void (*phandler)(int);

    /* connect to the EsounD server */
    sock = esd_open_sound( host );
    if ( sock < 0 ) 
	return sock;
    
    /* prepare the name buffer */
    if ( name )
	strncpy( name_buf, name, ESD_NAME_MAX );
    else
	name_buf[ 0 ] = '\0';

/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
    /* send the audio format information */
    if ( write( sock, &proto, sizeof(proto) ) != sizeof(proto) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    if ( write( sock, &format, sizeof(format) ) != sizeof(format) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    if( write( sock, &rate, sizeof(rate) ) != sizeof(rate) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    if( write( sock, name_buf, ESD_NAME_MAX ) != ESD_NAME_MAX ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    /* Reduce buffers on sockets to the minimum needed */
    esd_set_socket_buffers( sock, format, rate, 44100 );

    /* flush the socket */
    /* fsync( sock ); */
    
    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound recording stream\n" );
    */

    signal( SIGPIPE, phandler ); 
    return sock;
}

/**
 * esd_record_stream_fallback: esd_record_stream() but connect direct if no ESD
 * @format: data format for this stream
 * @rate: sample rate for this stream
 * @host: host to connect to, as for esd_open_sound().
 * @name: name by which to identify this stream to ESD.
 *
 * Attempts to create a connection to an ESD, using esd_record_stream(), and if
 * this fails falls back to trying to contact the soundcard directly.
 * (This will not work unless the soundcard is local.)
 *
 * Return Value: -1 on error, else a socket number to which the external
 * audio data arriving at the appropriate soundcard will be sent.
 */
int esd_record_stream_fallback( esd_format_t format, int rate, 
				const char *host, const char *name )
{
    int socket_out;

    /* try to open a connection to the server */
    if (!host) host = getenv("ESPEAKER");
    socket_out = esd_record_stream( format, rate, host, name );
    if ( socket_out >= 0 ) 
	return socket_out;

    /* if ESPEAKER is set, this is an error, bail out */
    if ( host )
	return -1;

    /* go for /dev/dsp */
    esd_audio_format = format;
    esd_audio_rate = rate;
    socket_out = esd_audio_open();

    /* Reduce buffers on sockets to the minimum needed */
    esd_set_socket_buffers( socket_out, format, rate, 44100 );

    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound recording stream fallback\n" );
    */

    /* we either got it, or we didn't */
    return socket_out;
}

/*******************************************************************/
/* cache a sample in the server returns sample id, <= 0 is error */
int esd_sample_cache( int esd, esd_format_t format, const int rate, 
		      const int size, const char *name )
{
    int id = 0;
    int proto = ESD_PROTO_SAMPLE_CACHE;
    void (*phandler)(int);

    /* prepare the name buffer */
    char name_buf[ ESD_NAME_MAX ];
    if ( name )
	strncpy( name_buf, name, ESD_NAME_MAX );
    else
	name_buf[ 0 ] = '\0';
    /* printf( "caching sample: %s (%d) - %ld bytes\n", 
       name_buf, esd, size ); */

/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
    /* send the necessary information */
    if ( write( esd, &proto, sizeof(proto) ) != sizeof(proto) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    if ( write( esd, &format, sizeof(format) ) != sizeof(format) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    if ( write( esd, &rate, sizeof(rate) ) != sizeof(rate) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    if ( write( esd, &size, sizeof(size) ) != sizeof(size) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    if ( write( esd, name_buf, ESD_NAME_MAX ) != ESD_NAME_MAX ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    /* flush the socket */
    /* fsync( esd ); */

    /* get the sample id back from the server */
    if ( read( esd, &id, sizeof(id) ) != sizeof(id) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound caching sample\n" );
    */

    /* return the sample id to the client */
    signal( SIGPIPE, phandler ); 
    return id;
}

/*******************************************************************/
/* call this after sending the sample data to the server, should */
/* return the same sample id read previously, <= 0 is error */
int esd_confirm_sample_cache( int esd )
{
    int id = 0;
    void (*phandler)(int);

/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
    /* get the sample id back from the server */
    if ( read( esd, &id, sizeof(id) ) != sizeof(id) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound confirming cached sample\n" );
    */

    /* return the sample id to the client */
    signal( SIGPIPE, phandler ); 
    return id;
}

/*******************************************************************/
/* get the sample ID for an already-cached sample */
int esd_sample_getid( int esd, const char *name)
{
    int proto = ESD_PROTO_SAMPLE_GETID;
    int id;
    char namebuf[ESD_NAME_MAX];
    void (*phandler)(int);

/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
    if ( write( esd, &proto, sizeof(proto) ) != sizeof(proto) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    /* prepare the name buffer */
    if ( name )
	strncpy( namebuf, name, ESD_NAME_MAX );
    else
	namebuf[ 0 ] = '\0';

    if ( write( esd, namebuf, ESD_NAME_MAX ) != ESD_NAME_MAX ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    /* flush the socket */
    /* fsync( esd ); */

    /* get the sample id back from the server */
    if ( read( esd, &id, sizeof(id) ) != sizeof(id) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound getting cached sample id: \'%s\' = %d\n",
      name, id );
    */
    
    /* return the sample id to the client */
    signal( SIGPIPE, phandler ); 
    return id;
}

/*******************************************************************/
/* uncache a sample in the server */
int esd_sample_free( int esd, int sample )
{
    int id;
    int proto = ESD_PROTO_SAMPLE_FREE;
    void (*phandler)(int);

    /* printf( "freeing sample (%d) - <%d>\n", esd, sample ); */

/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
    /* send the necessary information */
    if ( write( esd, &proto, sizeof(proto) ) != sizeof(proto) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    if ( write( esd, &sample, sizeof(sample) ) != sizeof(sample) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    /* fsync( esd ); */

    /* get the sample id back from the server */
    if ( read( esd, &id, sizeof(id) ) != sizeof(id) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound freeing sample\n" );
    */

    /* return the id to the client (0 = error, 1 = ok) */
    signal( SIGPIPE, phandler ); 
    return id;
}

/*******************************************************************/
/* uncache a sample in the server */
int esd_sample_play( int esd, int sample )
{
    int is_ok;
    int proto = ESD_PROTO_SAMPLE_PLAY;
    void (*phandler)(int);

    /* printf( "playing sample (%d) - <%d>\n", esd, sample ); */

/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
    /* send the necessary information */
    if ( write( esd, &proto, sizeof(proto) ) != sizeof(proto) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    if ( write( esd, &sample, sizeof(sample) ) != sizeof(sample) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    /* fsync( esd ); */

    /* get the sample id back from the server */
    if ( read( esd, &is_ok, sizeof(is_ok) ) != sizeof(is_ok) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound playing sample\n" );
    */

    /* return the id to the client (0 = error, 1 = ok) */
    signal( SIGPIPE, phandler ); 
    return is_ok;
}


/*******************************************************************/
/* loop a previously cached sample in the server */
int esd_sample_loop( int esd, int sample )
{
    int is_ok;
    int proto = ESD_PROTO_SAMPLE_LOOP;
    void (*phandler)(int);

    /* printf( "looping sample (%d) - <%d>\n", esd, sample ); */

/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
    /* send the necessary information */
    if ( write( esd, &proto, sizeof(proto) ) != sizeof(proto) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    if ( write( esd, &sample, sizeof(sample) ) != sizeof(sample) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    /* fsync( esd ); */

    /* get the sample id back from the server */
    if ( read( esd, &is_ok, sizeof(is_ok) ) != sizeof(is_ok) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound looping sample\n" );
    */

    /* return the id to the client (0 = error, 1 = ok) */
    signal( SIGPIPE, phandler ); 
    return is_ok;
}

/*******************************************************************/
/* stop a looping sample in the server */
int esd_sample_stop( int esd, int sample )
{
    int is_ok;
    int proto = ESD_PROTO_SAMPLE_STOP;
    void (*phandler)(int);

    /* printf( "stopping sample (%d) - <%d>\n", esd, sample ); */

/* this is unavoidable - incase ESD "dissapears" (ie the socket conn dies) */
/* we need to catch SIGPIPE to avoid the default handler form giving us */
/* a bad day - ignore the SIGPIPE, then make sure to cathc all errors */
    phandler = signal( SIGPIPE, dummy_signal );    /* for closed esd conns */
    /* send the necessary information */
    if ( write( esd, &proto, sizeof(proto) ) != sizeof(proto) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    if ( write( esd, &sample, sizeof(sample) ) != sizeof(sample) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }
    /* fsync( esd ); */

    /* get the sample id back from the server */
    if ( read( esd, &is_ok, sizeof(is_ok) ) != sizeof(is_ok) ) {
	signal( SIGPIPE, phandler ); 
	return -1;
    }

    /* diagnostic info */
    /*
      if ( getenv( "ESDBG" ) )
      printf( "esound stopping sample\n" );
    */

    /* return the id to the client (0 = error, 1 = ok) */
    signal( SIGPIPE, phandler ); 
    return is_ok;
}

/*******************************************************************/
/* closes fd, previously obtained by esd_open */
int esd_close( int esd )
{
    /* diagnostic info */
    /*
    if ( getenv( "ESDBG" ) )
	printf( "esound closing\n" );
    */

    return close( esd );
}
