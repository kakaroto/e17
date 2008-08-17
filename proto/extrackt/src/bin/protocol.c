/* vim: set ts=8 sw=3 sts=8 noexpandtab cino=>5n-3f0^-2{2: */
/* protocol.c
 * ==========
 * interprets the data that arrives from the pipe 
 */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Extrackt.h"

/* parse a message from the pipe an execute the corresponding 
 * functions. the protocol might be something like this:
 * header char[4]
 * **************
 * RIPE => rip end
 * RIPP => rip progress
 * RIPS => rip start
 * ENCE => encoder end
 * ENCP => we cant know the real
 * progress here without knowing the encoding quality.
 * ENCS => encoder start
 * 
 * S: start
 * E: end
 * body char *
 * ***********
 */
void
ex_protocol_parse(Extrackt *ex, char *msg, int length)
{
   char *oldmsg;

   oldmsg = msg;
   //printf("%d bytes of data received %s\n", length, msg);
   while(length > 0)
     {
	char *line;
	char header[5];
	char *body;
	int llength;

	line = E_STRDUP(msg);
	llength = strlen(msg);
	
	snprintf(header, sizeof(header),"%s", line);
	body = index(line,' ') + 1;
	
	/* FIXME the string received from the CDDB protocol
	 * might be incomplete (truncated) search the end of
	 * message (. the dot) if it isnt found accumulate the
	 * data
	 */
	
	/* the cddb process status */
	if(!strcmp("CDPS",header))
	  {
	     /* FIXME depending on the body set the status DONE or CANT_BE_DONE */
	     ex->cddb.status = EX_STATUS_DONE;
	  }
	/* the first query to the cddb, the match find query */
	else if(!strcmp("DBMF", header))
	  {
	     ex->cddb.pid = 0;
	     ex_cddb_response_find_parse(ex,body);
	  }
	/* the list info query */
	else if(!strcmp("DBMG", header))
	  {
	     ex->cddb.pid = 0;
	     ex_cddb_response_get_parse(ex,body);
	  }
	else if(!strcmp("RIPS", header))
	  {
	     time(&(ex->rip.start));
	     ex->rip.status = EX_STATUS_DOING;

	  }
	else if(!strcmp("RIPE", header))
	  {
	     ex->rip.status = EX_STATUS_DONE;
	     if(ex->rip.num_done + 1 != ex->rip.num_total)
	       ex->rip.num_done++;
	     else
	       {
		  ex->rip.num_done = 0;
		  ex->rip.num_total= 0;
		  //ex->rip.status = EX_STATUS_NOT_DONE;
	       }
	     ex->rip.pid = 0;
	     if(ex->encode.on)
	       ex_command_encode_append(ex, ex->rip.curr_track.number);
	     ex->rip.curr_track.number = -1;
	     ex->rip.curr_track.size = -1;
	     ex->rip.curr_track.filename = NULL;
	     ex->rip.tracks = evas_list_remove_list(ex->rip.tracks,ex->rip.tracks);
	     if(ex->rip.tracks)
	       ex_command_rip(ex);
	  }
	else if(!strcmp("RIPP", header))
	  {
	     double percent;

	     percent = atof(body);
	     ex->fe->rip_percent_update(percent);
	  }
	else if(!strcmp("ENCS", header))
	  {
	     ex->encode.status = EX_STATUS_DOING;

	  }
	else if(!strcmp("ENCE", header))
	  {
	     ex->encode.status = EX_STATUS_DONE;
	     if(ex->encode.num_done + 1 != ex->encode.num_total)
	       ex->encode.num_done++;
	     else
	       {
		  ex->encode.num_done = 0;
		  ex->encode.num_total= 0;
		  //ex->encode.status = EX_STATUS_NOT_DONE;
	       }
	     if(ex->config.encode->wav_delete)
	       ex_string_file_delete(ex->encode.curr_track.filename);
	     ex->encode.pid = 0;
	     ex->encode.curr_track.number = -1;
	     ex->encode.curr_track.size = -1;
	     ex->encode.curr_track.filename = NULL;
	     ex->encode.tracks = evas_list_remove_list(ex->encode.tracks,ex->encode.tracks);
	     if(ex->encode.tracks)
	       ex_command_encode(ex);
	  }
	else
	  {
	     /* printf("HEADER %s\n", header); */
	  }
	
	E_FREE(line);
	length = length - llength - 1; /* 1 = \n */
	msg = msg + llength + 1;
     }
}

void
ex_protocol_send(int fd, char *header, char *body)
{
   char *msg; 
  
   msg = E_NEW(strlen(header) + strlen(body) + 2, char);
   sprintf(msg,"%s %s",header,body);
   if(write(fd,msg, strlen(msg) + 1) < 0)
     printf("error writing to pipe\n");
}
