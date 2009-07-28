/* vim: set ts=8 sw=3 sts=8 noexpandtab cino=>5n-3f0^-2{2: */
/* commands.c
 * ==========
 * translate %tokens into real values for commands like "lame -s %A %l"
 * fork and run a command
 */

#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Extrackt.h"

#define STRING_ESCAPE \
   if(escape) \
   { \
      char *tmp; \
      tmp = astr; \
      astr = ex_string_escape(tmp); \
   }
   
/* local subsystem internal functions */
/**************************************/
char *_ex_command_translate(char *istr, Extrackt *ex, Ex_Config_Exe_Type ext, int escape);
void  _ex_command_rip_sigterm(int i);
void  _ex_command_encode_sigterm(int i);

static int rip_pid;
static int enc_pid;

/* FIXME make a list of chars that need to be replaced/escaped */
char *
ex_string_escape(char *str)
{

   char *buf;
   char *c;
   int n;
   int escaped;

   n = strlen(str) + 1;
   buf = E_NEW(n, char);
   n = 0;

   escaped = 0;
   for(c = str; *c; c++)
     {
	int i;
	i = *c;
	if( (i < 48) || (i > 90 && i < 97) || (i > 122 && i < 125) )
	  {
	     if(!escaped)
	       buf[n] = '_';
	     else
	       n--;  /* decrease by one to stay on the current n */
	     escaped = 1;
	  }
	else
	  {
	     buf[n] = *c;
	     escaped = 0;
	  }
	n++;
     }
   return buf;
}

void
ex_string_dir_make(char *path)
{
   char *dir;
   char *c;
   int len;

   dir = E_NEW(strlen(path)+1, char);
   for(c = path,len = 1; *c; c++,len++)
     {
	if(*c == '/')
	  {
	     strncpy(dir,path,len);
	     dir[len] = '\0';
	     printf("found / in %s\n", dir);
	     if(!ex_string_file_exists(dir))
	       {
		  mkdir(dir,0755);
		  printf("creating the dir %s\n", dir);
	       }
	  }
     }
}

int
ex_string_file_exists(char *path)
{
   struct stat st;
   int ret;

   ret = stat(path,&st);
   if(!ret)
     {
	printf("ok\n");
	return 1;
     }
   else if(errno == ENOENT)
     {
	return 0;
     }
   else
     {
	return -1;
     }
}

char *
ex_string_file_extension_get(char *file)
{
   char *ext;

   ext = rindex(file,'.') + 1;
   return strdup(ext);
}

void
ex_string_file_delete(char *file)
{
   if(remove(file) < 0)
     printf("error deleting file\n");
}

/* will tranlsate something like %e/%A/%t.mp3 (istr) => 
 * mp3/Bob\ Marley/01.mp3 (ostr) */
char **
ex_command_translate(Extrackt *ex, Ex_Config_Exe_Type ext)
{
   char *istr;
   char *l;
   
   char **argv;
   int argc;
 	
   Ex_Config_Exe *exe;

   /* put the exe on the first element of the array */
   argv = (char **)E_NEW(1, char *);
   	
   if(ext == EX_CONFIG_EXE_RIPPER)
     {
	exe = ex->rip.ripper;
     }
   else
     {
	exe = ex->encode.encoder;
     }
	
   argv[0] = E_STRDUP(exe->exe);
   argc = 1;
   
   istr = E_STRDUP(exe->command_line_opts);
   for(l = strtok(istr, " "); l; l = strtok(NULL," "))
     {
	/* allocate space for a new pointer */
	argv = (char **)realloc(argv, sizeof(char *) * (argc + 1));
	argv[argc] = _ex_command_translate(l,ex,ext,0);
	printf("argv %s\n", argv[argc]);
	argc++;
     }
   /* append a null at the end */
   argv = (char **)realloc(argv,sizeof(char *) * (argc + 1));
   argv[argc] = NULL;

   return argv;
}

/* FIXME we need to differentiate in case we should do escaping (file output), 
 * and not (id3, etc)
 */

char *
_ex_command_translate(char *istr, Extrackt *ex, Ex_Config_Exe_Type ext, int escape)
{
   char *buf;
   char *c;
   int bsize;
   int trans;
   int i;
   char *ostr;
   int *tracknumber;
   
   Ex_Config_Exe *exe;
   
   if(ext == EX_CONFIG_EXE_RIPPER)
     {	
	exe = ex->rip.ripper;
	tracknumber = ex->rip.tracks->data;
     }
   else
     {	
	exe = ex->encode.encoder;
	tracknumber = ex->encode.tracks->data;
     }
   
   
   /* create the buffer of 256 chars length */
   bsize = sizeof(char) * 256;
   buf = E_NEW(256, char);
   i = 0;
   trans = 0;
   /* actually translate the string */
   for(c = istr; *c; c++)
     {
	if( i > bsize - 1)
	  {
	     bsize += sizeof(char) * 256;
	     buf = realloc(buf, bsize);
	  }
	
	if(trans)
	  {	     
	     Ex_Track_Data *tdata;
	     Ex_Disc_Data *ddata;
	     Ex_Disc_Info *dinfo;
	     char *astr = NULL;
	     
	     dinfo = &(ex->disc_info);
	     ddata = &(ex->disc_data);
	     tdata = &(ddata->track[*tracknumber-1]);
	     if(*c == FMT_TRACK_NUM)
		  astr = tdata->number;
	     else if(*c == FMT_DISC_TITLE)
	       {
		  astr = ddata->title;
		  STRING_ESCAPE;
	       }
	     else if(*c == FMT_DISC_ARTIST)
	       {
		  astr = ddata->artist;
		  STRING_ESCAPE;
	       }
	     else if(*c == FMT_DISC_YEAR)
	       {
		  astr = E_NEW(5, char); /* 4 chars + /0 */
		  snprintf(astr,5,"%d",ddata->year);
	       }
	     else if(*c == FMT_DISC_GENRE_NUM)  
	       {
		  astr = E_NEW(5, char); /* 4 chars + /0 */
		  snprintf(astr,5,"%d",ddata->id3genre);
	       }
	     else if(*c == FMT_DISC_GENRE_STRING)
	       astr = ex_id3_genre_string(ddata->id3genre);
	     else if(*c == FMT_DISC_DEVICE)
	       astr = ex->config.cd->cdrom;
	     else if(*c == FMT_TRACK_TITLE)
	       {
		  astr = tdata->name;
		  STRING_ESCAPE;
	       }
	     else if(*c == FMT_TRACK_ARTIST)
	       {
		  if(*(tdata->artist))
		    astr = tdata->artist;
		  else
		    astr = ddata->artist;
		  STRING_ESCAPE;
	       }
	     else if(*c == FMT_OUTPUT_FILE)
	       {
		  
		  astr = _ex_command_translate(exe->file_format,ex,ext,1);
		  if(exe->type == EX_CONFIG_EXE_RIPPER)
		    {
		       if(tdata->wav_filename)
			 E_FREE(tdata->wav_filename);
		       
		       tdata->wav_filename = E_STRDUP(astr);
		    }
		  /* check if the output dir actually exists, if not create it */
		  ex_string_dir_make(astr);
		  
	       }
	     else if(*c == FMT_INPUT_WAV)
	       astr = tdata->wav_filename;
	     else if(*c == FMT_FILE_EXT)
	       astr = ex_string_file_extension_get(exe->file_format);
	     else
	       printf("error on tokens\n");
	     
	     if(bsize < i + strlen(astr))
	       {
		  bsize += strlen(astr) + 1;
		  buf = realloc(buf, bsize);
	       }
	     buf[i-1] = '\0';
	     sprintf(buf, "%s%s", buf, astr);
	     i += strlen(astr) - 1;
	     trans = 0;
	     continue;
	  }
	if(*c == '%')
	  {
	     trans = 1;
	  }
	else if(*c == ' ')
	  {
	     buf[i] = '_';
	     
	  }
	else
	  {
	     buf[i] = *c;
	  }
	i++;
     }
   buf[i] = '\0';
   ostr = E_STRDUP(buf);
   E_FREE(buf);
   return ostr;   
}

void
ex_command_encode_append(Extrackt *ex, int tracknumber)
{
   int run = 0;
   int *tp;
   tp = E_NEW(1, int);
   *tp = tracknumber;
   
   printf("appending track number %d to encode\n", tracknumber);
   /* if its the first track to be appended also launch the encoder */
   if(!ex->encode.tracks)
     {
	ex->encode.num_queue = 0;
	run = 1;
     }
   ex->encode.tracks = eina_list_append(ex->encode.tracks, tp);
   ex->encode.num_queue++;
   if(run)
     {
	printf("first time appended, launch it\n");
	ex_command_encode(ex);
     }
}

void
ex_command_encode(Extrackt *ex)
{
   pid_t   pid;
   char **argv;
   int *tracknumber;
   Ex_Config_Exe *encode;
   Ex_Track_Info *tinfo;
   Ex_Track_Data *tdata;

   if(!ex->encode.tracks)
     return;
   
   tracknumber = ex->encode.tracks->data;
   
   encode = ex->encode.encoder;
   tinfo = &(ex->disc_info.track[*tracknumber-1]);
   tdata = &(ex->disc_data.track[*tracknumber-1]);
   
   argv = ex_command_translate(ex,EX_CONFIG_EXE_ENCODER);
   
   printf("encoding track %d\n", *tracknumber);

   pid = fork ();
   /* child */
   if(pid == 0)
     {
	pid_t   pid_sub;
	/* on the child we should split it on two again
	 * one for actually exec the command and other to stat
	 * the file
	 */
	pid_sub = fork();
	if (pid_sub == 0)
	  {
	     /* close the stdout,stderr,stdin */
	     close(0);
	     close(1);
	     close(2);
	     
	     ex_protocol_send(ex->pfd[1],"ENCS", "");
	     execvp(argv[0], argv);
	     exit(0);
	  }
	else if (pid_sub > 0)
	  {
	     int st;
	     
	     enc_pid = pid_sub;
	     signal(SIGTERM,_ex_command_encode_sigterm);
	     waitpid(pid_sub,&st,0);
	     if(!WIFSIGNALED(st))
	       ex_protocol_send(ex->pfd[1],"ENCE", "");
	     exit(0);
	  }
	else
	  {
	     /* error */
	     exit(0);
	  }
	
     }
   /* parent */
   else if(pid > 0)
     {
	ex->encode.pid = pid;
	ex->encode.curr_track.size = ex_cddev_track_size(tinfo);
	ex->encode.curr_track.number = *tracknumber;
	ex->encode.curr_track.filename = tdata->wav_filename;
	return;
     }
   /* error */
   else
     {
	/* debug it */
	return;
     }
   return;
}

int	
ex_command_encode_set(Extrackt *ex, char *name)
{
   Eina_List *l;
   
   for(l = ex->config.encode->encoders; l; l=l->next)
     {
	Ex_Config_Exe *ecx;
	ecx = l->data;

	if(!strcmp(ecx->name, name))
	  {
	     ex->encode.encoder = ecx;
	     return 1;
	  }
     }
   return 0;
}

void
ex_command_rip(Extrackt *ex)
{
   pid_t   pid;
   char **argv;
   int *tracknumber;
   Ex_Track_Data *tdata;
   Ex_Track_Info *tinfo;

   if(!ex->rip.tracks)
     return;

   tracknumber = ex->rip.tracks->data;
   
   tinfo = &(ex->disc_info.track[*tracknumber-1]);
   tdata = &(ex->disc_data.track[*tracknumber-1]);
   
   argv = ex_command_translate(ex,EX_CONFIG_EXE_RIPPER);
   
   printf("ripping track %d\n", *tracknumber);
      
   pid = fork ();
   /* child */
   if(pid == 0)
     {
	pid_t   pid_sub;
	/* on the child we should split it on two again
	 * one for actually exec the command and other to stat
	 * the file
	 */
	pid_sub = fork();
	if (pid_sub == 0)
	  {
	     /* close the stdout,stderr,stdin */
	     /* FIXME no closing makes the ripper to no function */
	     //close(0);
	     //close(1);
	     //close(2);
	     
	     ex_protocol_send(ex->pfd[1],"RIPS", "");
	     execvp(argv[0], argv);
	     exit(0);
	  }
	else if (pid_sub > 0)
	  {
	     int st;
	    
	     rip_pid = pid_sub;
	     signal(SIGTERM,_ex_command_rip_sigterm);
	     waitpid(pid_sub,&st,0);
	     if(!WIFSIGNALED(st))
	       ex_protocol_send(ex->pfd[1],"RIPE", "");
	     exit(0);
	     
	  }
	else
	  {
	     /* error */
	     exit(0);
	  }
     }
   /* parent */
   else if(pid > 0)
     {
	/* store the childs pid (the child that stats the file) */
	ex->rip.pid = pid;
	ex->rip.curr_track.size = ex_cddev_track_size(tinfo);
	ex->rip.curr_track.number = *tracknumber;
	ex->rip.curr_track.filename = tdata->wav_filename;
	return;
     }
   /* error */
   else
     {
	/* debug it */
	return;
     }
   return;
}

void
ex_command_rip_append(Extrackt *ex, int tracknumber)
{
   int *tp;

   tp = E_NEW(1, int);
   *tp = tracknumber;
   printf("appending track number %d\n", tracknumber);
   ex->rip.tracks = eina_list_append(ex->rip.tracks, tp);
   ex->rip.num_total++;
   if(ex->encode.on)
     ex->encode.num_total++;
}

int
ex_command_rip_update(Extrackt *ex)
{
   double percent;
   char *percentstring;
   struct stat s;

   switch(ex->rip.status)
     {
      case EX_STATUS_DOING:

	/* if the file doesnt exists or it hasnt been written */
	if( (stat(ex->rip.curr_track.filename,&s) < 0) || (difftime(ex->rip.start,s.st_mtime) > 0))
	  return 0;
	/* the size of the ripped file / size of the wav */
	percent = (float)s.st_size/(float)ex->rip.curr_track.size;
	/* sometimes the ripper doesnt rips everything
	 * so check if the ripper continues or not */
	if(percent >= 1.0)
	  percent = 1.0;
	else if(percent <= 0.01)
	  percent = 0.0;
	break;
      
      case EX_STATUS_DONE:
	percent = 1.0;
	break;
      
      case EX_STATUS_NOT_DONE:
	percent = 0.0;
	break;
      
      default :
	return 0;

     }
   percentstring = E_NEW(10, char); // FIXME THIS
   snprintf(percentstring,10,"%g",percent);
   ex_protocol_send(ex->pfd[1],"RIPP", percentstring);

   return 1;
}

int	
ex_command_rip_set(Extrackt *ex, char *name)
{
   Eina_List *l;
   
   for(l = ex->config.rippers; l; l=l->next)
     {
	Ex_Config_Exe *ecx;
	ecx = l->data;

	if(!strcmp(ecx->name, name))
	  {
	     ex->rip.ripper = ecx;
	     return 1;
	  }
     }
   return 0;
}

void
ex_command_rip_abort(Extrackt *ex)
{
   if(!ex->rip.pid)
     return;
   kill(ex->rip.pid,SIGTERM);
   ex->rip.pid = 0;
   ex->rip.num_done = 0;
   ex->rip.num_total = 0;
   ex->rip.status = EX_STATUS_NOT_DONE;
	     
   ex->rip.curr_track.number = -1;
   ex->rip.curr_track.size = -1;
   ex->rip.curr_track.filename = NULL;
   
   while(ex->rip.tracks)
     ex->rip.tracks = eina_list_remove_list(ex->rip.tracks,ex->rip.tracks);
}

void
ex_command_encode_abort(Extrackt *ex)
{
   if(!ex->encode.pid)
     return;
   kill(ex->encode.pid,SIGTERM);
   ex->encode.pid = 0;
   ex->encode.num_done = 0;
   ex->encode.num_total = 0;
   ex->encode.num_queue = 0;
   ex->encode.status = EX_STATUS_NOT_DONE;
   
   ex->encode.curr_track.number = -1;
   ex->encode.curr_track.size = -1;
   ex->encode.curr_track.filename = NULL;
   
   while(ex->encode.tracks)
     ex->encode.tracks = eina_list_remove_list(ex->encode.tracks,ex->encode.tracks);
}

void
_ex_command_encode_sigterm(int i)
{
   /* to be fair also kill his child */
   kill(enc_pid,SIGKILL);
   exit(0);
}

void
_ex_command_rip_sigterm(int i)
{
   /* to be fair also kill his child */
   kill(rip_pid,SIGKILL);
   exit(0);
}

