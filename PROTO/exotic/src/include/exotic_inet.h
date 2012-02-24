#ifndef EXOTIC_INET_H_
# define EXOTIC_INET_H_

#ifdef EXOTIC_PROVIDE_INET

#ifdef htonl
# undef htonl
#endif
#define htonl exotic_htonl

EAPI unsigned int exotic_htonl(unsigned int hostlong);

#ifdef ntohl
# undef ntohl
#endif
#define ntohl exotic_ntohl
EAPI unsigned int exotic_ntohl(unsigned int netlong);

#endif

#endif /* EXOTIC_INET_H_ */
