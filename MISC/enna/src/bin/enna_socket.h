#ifndef _ENNA_SOCKET_H_
# define _ENNA_SOCKET_H_

EAPI int            enna_connect(char *hostname, int port, int block, char *err,
				 int err_len);

#endif
