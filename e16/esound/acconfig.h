#undef DRIVER_OSS
#undef DRIVER_AIX
#undef DRIVER_IRIX
#undef DRIVER_HPUX
#undef DRIVER_SOLARIS
#undef DRIVER_MKLINUX
#undef DRIVER_ALSA
#undef DRIVER_NEWALSA
#undef DRIVER_NONE
#undef HAVE_INET_ATON
#undef HAVE_NANOSLEEP
#undef USE_LIBWRAP
#undef WITH_SYMBOL_UNDERSCORE
#undef ESDBG 

#undef INADDR_LOOPBACK
#undef HAVE_SUN_LEN



@BOTTOM@
/* keep these at the end of the generated config.h */

#ifndef HAVE_SUN_LEN
#define SUN_LEN(ptr) ((size_t)(((struct sockaddr_un *) 0)->sun_path) + strlen ((ptr)->sun_path))
#endif
