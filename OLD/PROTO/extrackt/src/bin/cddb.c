/* vim: set ts=8 sw=3 sts=8 noexpandtab cino=>5n-3f0^-2{2: */
/* cddb.c
 * *******
 * functions to connect to a cddb server
 * retreive the data from a cddb server
 * talk the cddb protocol
 * for reference: http://www.freedb.org/modules.php?name=Sections&sop=listarticles&secid=2
 */

#include "Extrackt.h"   

static int   _ex_cddb_sum(int val);
static void  _ex_cddb_line_process(char *inbuffer, Ex_Disc_Data *data, int numtracks);
static char *_ex_cddb_line_data_get(char **dataptr);
static char *_ex_cddb_uri_make(Ex_Config_Cddb_Server *server, Ex_Cddb_Hello *hello, char *cmd);    
static char *_ex_cddb_request_make(Ex_Config_Cddb_Server *server,Ex_Cddb_Hello *hello, char *cmd);

static char *discdb_genres[]={"unknown","blues","classical","country",
     "data","folk","jazz","misc","newage",
     "reggae","rock","soundtrack"
};


/* this array contains string representations of all known ID3 tags */
/* taken from mp3id3 in the mp3tools 0.7 package */

static Ex_Id3_Genre id3_genres[] = {
  {"Alternative",20},
  {"Anime",146},
  {"Blues",0},
  {"Classical",32},
  {"Country",2},
  {"Folk",80},
  {"Jazz",8},
  {"Metal",9},
  {"Pop",13},
  {"Rap",15},
  {"Reggae",16},
  {"Rock",17},
  {"Other",12},
  {"Acapella",123},
  {"Acid",34},
  {"Acid Jazz",74},
  {"Acid Punk",73},
  {"Acoustic",99},
  {"AlternRock",40},
  {"Ambient",26},
  {"Avantgarde",90},
  {"Ballad",116},
  {"Bass",41},
  {"Beat",135},
  {"Bebob",85},
  {"Big Band",96},
  {"Black Metal",138},
  {"Bluegrass",89},
  {"Booty Bass",107},
  {"BritPop",132},
  {"Cabaret",65},
  {"Celtic",88},
  {"Chamber Music",104},
  {"Chanson",102},
  {"Chorus",97},
  {"Christian Gangsta Rap",136},
  {"Christian Rap",61},
  {"Christian Rock",141},
  {"Classic Rock",1},
  {"Club",112},
  {"Club-House",128},
  {"Comedy",57},
  {"Contemporary Christian",140},
  {"Crossover",139},
  {"Cult",58},
  {"Dance",3},
  {"Dance Hall",125},
  {"Darkwave",50},
  {"Death Metal",22},
  {"Disco",4},
  {"Dream",55},
  {"Drum & Bass",127},
  {"Drum Solo",122},
  {"Duet",120},
  {"Easy Listening",98},
  {"Electronic",52},
  {"Ethnic",48},
  {"Eurodance",54},
  {"Euro-house",124},
  {"Euro-Techno",25},
  {"Fast Fusion",84},
  {"Folklore",115},
  {"Folk/Rock",81},
  {"Freestyle",119},
  {"Funk",5},
  {"Fusion",30},
  {"Game",36},
  {"Gangsta",59},
  {"Goa",126},
  {"Gospel",38},
  {"Gothic",49},
  {"Gothic Rock",91},
  {"Grunge",6},
  {"Hardcore",129},
  {"Hard Rock",79},
  {"Heavy Metal",137},
  {"Hip-Hop",7},
  {"House",35},
  {"Humour",100},
  {"Indie",131},
  {"Industrial",19},
  {"Instrumental",33},
  {"Instrumental Pop",46},
  {"Instrumental Rock",47},
  {"Jazz+Funk",29},
  {"JPop",145},
  {"Jungle",63},
  {"Latin",86},
  {"Lo-Fi",71},
  {"Meditative",45},
  {"Merengue",142},
  {"Musical",77},
  {"National Folk",82},
  {"Native American",64},
  {"Negerpunk",133},
  {"New Age",10},
  {"New Wave",66},
  {"Noise",39},
  {"Oldies",11},
  {"Opera",103},
  {"Polka",75},
  {"Polsk Punk",134},
  {"Pop-Folk",53},
  {"Pop/Funk",62},
  {"Porn Groove",109},
  {"Power Ballad",117},
  {"Pranks",23},
  {"Primus",108},
  {"Progressive Rock",92},
  {"Psychadelic",67},
  {"Psychedelic Rock",93},
  {"Punk",43},
  {"Punk Rock",121},
  {"Rave",68},
  {"R&B",14},
  {"Retro",76},
  {"Revival",87},
  {"Rhythmic Soul",118},
  {"Rock & Roll",78},
  {"Salsa",143},
  {"Samba",114},
  {"Satire",110},
  {"Showtunes",69},
  {"Ska",21},
  {"Slow Jam",111},
  {"Slow Rock",95},
  {"Sonata",105},
  {"Soul",42},
  {"Sound Clip",37},
  {"Soundtrack",24},
  {"Southern Rock",56},
  {"Space",44},
  {"Speech",101},
  {"Swing",83},
  {"Symphonic Rock",94},
  {"Symphony",106},
  {"SynthPop",147},
  {"Tango",113},
  {"Techno",18},
  {"Techno-Industrial",51},
  {"Terror",130},
  {"Top 40",60},
  {"Trailer",70},
  {"Trance",31},
  {"Trash Metal",144},
  {"Tribal",72},
  {"Trip-Hop",27},
  {"Vocal",28},
  {NULL,145}
};

/* This array maps CDDB_ genre numbers to closest id3 genre */
int cddb_2_id3[] =
{
  12,         /* CDDB_UNKNOWN */
  0,          /* CDDB_BLUES */
  32,         /* CDDB_CLASSICAL */
  2,          /* CDDB_COUNTRY */
  12,         /* CDDB_DATA */
  80,         /* CDDB_FOLK */
  8,          /* CDDB_JAZZ */
  12,         /* CDDB_MISC */
  10,         /* CDDB_NEWAGE */
  16,         /* CDDB_REGGAE */
  17,         /* CDDB_ROCK */
  24,         /* CDDB_SOUNDTRACK */
};

void
ex_cddb_default_set(Ex_Disc_Data *dd, int numtracks)
{
   int t;

   for(t=0; t < numtracks; t++)
     {
     
	sprintf(dd->track[t].name,_("Track %d"), t + 1);
	sprintf(dd->track[t].number,"%d", t + 1);
	*(dd->track[t].artist) = '\0';
     }

   sprintf(dd->title,_("Unknown Disk"));
   sprintf(dd->artist, _("Unknown Artist"));
   dd->year = 0;
   dd->genre = 7;
   dd->id3genre = -1;
}

/* will receive the header of our own protocol, the request 
 * to the cddb server. it makes the comunication process
 * in a child
 */
void
ex_cddb_request_send(Extrackt *ex, char *header, char *request)
{
   pid_t   pid;
   char *response;
      
   pid = fork ();
   /* child */
   if(pid == 0)
     {
	char  rcode[4];
	/* close the read fd, we dont need it */
	close(ex->pfd[0]);
	/* send the request to the cddb server */
	response = _ex_cddb_request_make(ex->config.cddb->primary, ex->cddb.hello, request);
	/* get thre response code */
	snprintf(rcode, sizeof(rcode), "%s", response);
	switch(rcode[0])
	  {
	     /* 2xx the command is ok */
	   case '2':
		{
		   char *data;

		   data = index(response, ' ') + 1;
		   ex_protocol_send(ex->pfd[1],header, data);
		  		
		}
	       /* send a DONE status */
	      ex_protocol_send(ex->pfd[1],"CDPS","1");
	      break;
	      /* everything is an error */
	   default:
	      printf("cddb error code %s\n",rcode);
	      /* send a CANT_BE_DONE status */
	      ex_protocol_send(ex->pfd[1],"CDPS","0");
	      break;
	  }
	exit(0);
     }
   /* parent */
   else if(pid > 0)
     {
	/* store the childs pid */
	ex->cddb.pid = pid;
	/* we are currently sending the query */
	ex->cddb.status = EX_STATUS_DOING;
	return;
     }
   /* error */
   else
     {
	/* debug it */
	return;
     }
}

/* find all the discs that match */
void
ex_cddb_match_find(Extrackt *ex)
{
   int   i;
   char request[1024];  // FIXME this can be done dinamically

   snprintf(request, sizeof(request), "cddb+query+%08x+%d", ex->disc_id,
	     ex->disc_info.num_tracks);
   
   for(i=0; i < ex->disc_info.num_tracks; i++)
     sprintf(request, "%s+%d", request, ex->disc_info.track[i].start_frame);
   
   sprintf(request, "%s+%d",request, ex->disc_info.length.mins*60 + 
	    ex->disc_info.length.secs);
   
   ex_cddb_request_send(ex,"DBMF",request);      
}

/* get the disc info associated with the list id */
void
ex_cddb_match_get(Extrackt *ex, char *genre, char *list_id)
{
   char *request;

   request = E_NEW(strlen(genre)+strlen(list_id)+strlen("cddb+read++")+1, char);
   sprintf(request, "cddb+read+%s+%s", genre, list_id);

   ex_cddb_request_send(ex,"DBMG",request);   
}

void
ex_cddb_response_find_parse(Extrackt *ex, char *response)
{
   char *genre;
   char *id;
   char *line;
   
   while((line = _ex_cddb_line_data_get(&response)))
     {

	genre = strsep(&line," ");  // this is the genre
	id = strsep(&line, " "); // list id (or discid), the rest is the title

	/* store the info and request the data */
	ex_cddb_match_get(ex,genre,id);
     }  
}

void
ex_cddb_response_get_parse(Extrackt *ex, char *response)
{
   char *line;
   while((line = _ex_cddb_line_data_get(&response)))
     {
   
	_ex_cddb_line_process(line, &ex->disc_data, ex->disc_info.num_tracks);
     }
   /* update the frontend info */
   ex->fe->disc_update(ex->fe->data);
}

/* return the computed disc id */
unsigned int
ex_cddb_discid_get(Extrackt *ex)
{
   int index, tracksum = 0, discid;
   
   if(!ex->disc_info.have_info) ex_cddev_stat(ex, TRUE);
   
   for(index = 0; index < ex->disc_info.num_tracks; index++)
     tracksum += _ex_cddb_sum(ex->disc_info.track[index].start_pos.mins * 60 +
			      ex->disc_info.track[index].start_pos.secs);
   
   discid = (ex->disc_info.length.mins * 60 + ex->disc_info.length.secs) -
     (ex->disc_info.track[0].start_pos.mins * 60 + ex->disc_info.track[0].start_pos.secs);
   
   return (tracksum % 0xFF) << 24 | discid << 8 | ex->disc_info.num_tracks;   
}

/* Convert numerical genre to text */
char *
ex_cddb_genre_to_text(int genre)
{
   if(genre>11) return("unknown");   
   return discdb_genres[genre];
}

/* Convert genre from text form into an integer value */
int 
ex_cddb_genre_to_num(char *genre)
{
   int pos;
   
   for(pos=0; pos<12; pos++)
     if(!strcmp(genre, discdb_genres[pos])) return pos;
   
   return 0;
}

int
ex_id3_genre_value(char *genre)
{
   int i;
   for(i=0; id3_genres[i].name; i++)
     {
	if(!strcasecmp(genre,id3_genres[i].name))
	  return id3_genres[i].num;
     }
   return -1;
}

char*
ex_id3_genre_string(int num)
{
   int i;
   for(i=0; id3_genres[i].name; i++)
     {
	if(id3_genres[i].num == num)
	  return E_STRDUP(id3_genres[i].name);
     }
   return NULL;
}

/* local subsystem functions */
/*****************************/


/* Process a line of input data */
static void 
_ex_cddb_line_process(char *inbuffer, Ex_Disc_Data *data, int numtracks)
{
   int   track;
   int   len = 0;
   char *st;
  
   /* FIXME where do we free the inbuffer? */
   strtok(inbuffer, "\n\r");
   
   if(!strncasecmp(inbuffer, "# Revision: ", 12)) 
     {
	data->revision=atoi(inbuffer+12);
     }
   else if(!strncasecmp(inbuffer, "DTITLE", 6))
     {
	char *dtitle;
	char *artist;
	
	dtitle = inbuffer + 7;  
	artist = strsep(&dtitle, "/");
	
	snprintf(data->artist, strlen(artist), "%s", artist); /* remove one for the final space */
	sprintf(data->title, "%s", dtitle + 1); /* shit one for the initial space */
     }
   else if(!strncasecmp(inbuffer, "DYEAR", 5))
     {
	strtok(inbuffer, "=");	
	st = strtok(NULL, "");
	if(st == NULL)
	  return;	
	data->year = atoi(ex_util_strstrip(st));
     }
   else if(!strncasecmp(inbuffer, "DGENRE", 6))
     {
	strtok(inbuffer,"=");
	
	st = strtok(NULL, "");
	if(st == NULL)
	  return;
	
	st = ex_util_strstrip(st);
	
	if(*st) 
	  {
	     data->genre = ex_cddb_genre_to_num(st);
	     data->id3genre = ex_id3_genre_value(st);
	  }
     }
   else if(!strncasecmp(inbuffer, "DID3", 4))
     {
	strtok(inbuffer, "=");	
	st = strtok(NULL, "");
	if(st == NULL)
	  return;
	
	data->id3genre = atoi(ex_util_strstrip(st));
     }
   else if(!strncasecmp(inbuffer, "TTITLE", 6)) 
     {

	track =  atoi(strtok(inbuffer + 6, "="));
	     	
	if(track < numtracks)
	  len=strlen(data->track[track].name);
	
	st = strtok(NULL, "");
	if(st == NULL)
	  return;
		
	sprintf(data->track[track].name, "%s", st);
	
	len = strlen(data->track[track].number);
	sprintf(data->track[track].number, "%d", track+1);
     }
   else if(!strncasecmp(inbuffer, "TARTIST", 7)) 
     {
	data->multi_artist = TRUE;	
	track = atoi(strtok(inbuffer + 7, "="));	
	if(track < numtracks)
	  len = strlen(data->track[track].artist);
	
	st = strtok(NULL, "");
	if(st == NULL)
	  return;
	
	snprintf(data->track[track].artist + len, 256 - len, "%s", st);
     }
   else if(!strncasecmp(inbuffer, "EXTD", 4))
     {
	len = strlen(data->extended);	
	snprintf(data->extended + len, 4096 - len, "%s", inbuffer + 5);
     }
   else if(!strncasecmp(inbuffer, "EXTT", 4))
     {
	track = atoi(strtok(inbuffer+4, "="));
	
	if(track < numtracks)
	  len = strlen(data->track[track].extended);	
	st = strtok(NULL, "");
	if(st == NULL)
	  return;
	
	snprintf(data->track[track].extended + len, 4096 - len, "%s", st);
     }
   else if(!strncasecmp(inbuffer, "PLAYORDER", 5))
     {
	len = strlen(data->playlist);	
	snprintf(data->playlist + len, 256 - len, "%s", inbuffer + 10);
     }
}
					    
/* get the pointer to data from a line without the
 * \n \0 . (end of cddb protocol) 
 * */
static char *
_ex_cddb_line_data_get(char **line)
{
   char *data = *line;
   char *c;
   
   if(!data || !*data || *data=='.') 
     {
	*line = NULL;      
	return NULL;
     }
   for(c = data; *c; c++) 
     {
	if(*c =='\n') 
	  {
	     *c='\0';
	     *line = c + 1;
	     
	     return data;
	  }
     }
   *line = NULL;   
   return data;
}

static char *
_ex_cddb_uri_make(Ex_Config_Cddb_Server *server, Ex_Cddb_Hello *hello, char *cmd)
{
   char *uri;
   
   uri = E_NEW(512, char);
   
   snprintf(uri, 512, "http://%s/%s?cmd=%s&hello=private+free.the.cddb+%s+%s"
	    "&proto=%d",
	    server->name,server->cgi_prog,cmd,
	    hello->hello_program,hello->hello_version,
	    hello->proto_version);
   
   return uri;
}

static char *
_ex_cddb_request_make(Ex_Config_Cddb_Server *server,Ex_Cddb_Hello *hello, char *cmd)
{
   char  user_agent[256];
   char *uri;   
   char *data = NULL;
   
   uri = _ex_cddb_uri_make(server, hello, cmd);      
   printf("URI is %s\n", uri);
	
   snprintf(user_agent, 256, "User-Agent: %s %s", hello->hello_program,
	 hello->hello_version);
   data = ex_http_get(uri);
   return data;
}

/* sum function */  
static int 
_ex_cddb_sum(int val)
{
   char *bufptr, buf[16];
   int ret = 0;
   
   snprintf(buf,16,"%lu",(unsigned long int)val);
   
   for(bufptr = buf; *bufptr != '\0'; bufptr++)
     ret += (*bufptr - '0');
   
   return ret;
}  
