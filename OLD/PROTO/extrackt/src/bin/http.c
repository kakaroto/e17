/* vim: set ts=8 sw=3 sts=8 noexpandtab cino=>5n-3f0^-2{2: */
/*
 * http.c
 * ======
 * Some simple routines for connecting to a remote tcp socket
 */

#include "Extrackt.h"

int 
ex_http_connection_open(char * server, int port)
{	
   int sock;
#ifdef USE_IPV6
   struct addrinfo hints, *res, *res0;
   char service[6];
#else
   struct hostent *host;
   struct sockaddr_in address;
#endif

#ifdef USE_IPV6

   snprintf(service, 6, "%d", port);
   memset(&hints, 0, sizeof(hints));
   hints.ai_socktype = SOCK_STREAM;

   if (getaddrinfo(server, service, &hints, &res0))
     return 0;

   for (res = res0; res; res = res->ai_next) {
	sock = socket (res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock < 0) 
	  {
	     if (res->ai_next)
	       continue;
	     else 
	       {
		  freeaddrinfo(res0);
		  return 0;
	       }
	  }
	if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) 
	  {
	     if (res->ai_next) 
	       {
		  close(sock);
		  continue;
	       } 
	     else 
	       {
		  freeaddrinfo(res0);
		  return 0;
	       }
	  }
	freeaddrinfo(res0);
	return sock;
   }
#else
   sock = socket(AF_INET, SOCK_STREAM, 0);
   address.sin_family = AF_INET;

   if (!(host = gethostbyname(server)))
     return 0;

   memcpy(&address.sin_addr.s_addr, *(host->h_addr_list), sizeof(address.sin_addr.s_addr));
	
   address.sin_port = htons(port);

   if (connect(sock, (struct sockaddr *) &address, sizeof (struct sockaddr_in)) == -1)
     return 0;
#endif
   return sock;
}

void 
ex_http_connection_close(int sock)
{
   shutdown(sock, 2);
   close(sock);
}

int 
ex_http_line_read(int sock, char * buf, int size)
{
   int i = 0;
	
   while (i < size - 1)
     {
	if (read(sock, buf + i, 1) <= 0)
	  {
	     if (i == 0)
	       return -1;
	     else
	       break;
	  }
	if (buf[i] == '\n')
	     break;
	if (buf[i] != '\r')
	  i++;
     }
   //buf[i] = '\0';
   return i;
}

/* Skips the HTTP-header, if there is one, and reads the first line into buf.
 *  Returns number of bytes read. */
int 
ex_http_line_read_first(int sock, char * buf, int size)
{
   int i;
   
   /* Skip the HTTP-header */
   if ((i = ex_http_line_read(sock, buf, size)) < 0)
     return -1;
   if (!strncmp(buf, "HTTP", 4)) /* Check to make sure its not HTTP/0.9 */
     {
	while (ex_http_line_read(sock, buf, size) > 0)
	  /* nothing */;
	if ((i = ex_http_line_read(sock, buf, size)) < 0)
	  return -1;	
     }
   return i;
}

char * 
ex_http_get(char * url)
{
   char *server, getstr[1024], *buf = NULL, *bptr;
   char *gs, *gc, *turl = url;
   int sock, n, bsize, port = 0;
   
   /* Skip past ``http://'' part of URL */
   
   if ( !strncmp(turl, "http:", 5) ) 
     {
	turl += 5;
	if ( !strncmp(turl, "//", 2) )
	  turl += 2;
     }
   
   /* If path starts with a '/', we are referring to localhost */   
   if ( turl[0] == '/' )
     server = "localhost";
   
   else
     server = turl;
   
   /* Check if URL contains port specification */
   gc = strchr(turl, ':');
   gs = strchr(turl, '/');   
   
   if ( gc != NULL && gc < gs ) 
     {
	port = atoi(gc+1);
	*gc = '\0';
     }
   
   if ( port == 0 )
     port = 80;
   
   /* Make sure that server string is null terminated. */
   
   if ( gs )
     *gs = '\0';
      
   /*
    * Now, open connection to server.
    */
   
   sock = ex_http_connection_open(server, port);
   
   /* Repair the URL string that we broke earlier on */
   if ( gs )
     *gs = '/';
   
   if ( gc && gc == '\0' )
     *gc = ':';
   
   if ( sock == 0 )
     return NULL;
   
   /*
    * Send query to socket.
    */
   
   sprintf(getstr,"GET %s HTTP/1.0\r\n\r\n", gs ? gs : "/");
      
   if ( write(sock, getstr, strlen(getstr)) == -1 ) 
     {
	ex_http_connection_close(sock);
	return NULL;
     }
   
   /*
    * Start receiving result.
    */
   
   bsize = 4096;
   bptr = buf = E_NEW(bsize, char);
   if ( (n = ex_http_line_read_first(sock, bptr, bsize)) == -1 ) 
     {
	
	E_FREE(buf);
	buf = NULL;
	goto Done;
     }
   bsize -= n + 1;
   bptr += n + 1;
   
   while (bsize > 0 && (n = ex_http_line_read(sock, bptr, bsize)) != -1) 
     {
	bptr += n + 1;
	bsize -= n + 1;
     }
Done:
   ex_http_connection_close(sock);
   *bptr = '\0';
   /*
    * Return result buffer to caller.
    */
   return buf;
}
