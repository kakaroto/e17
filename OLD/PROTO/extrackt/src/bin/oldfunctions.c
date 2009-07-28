/* vim: set ts=8 sw=3 sts=8 noexpandtab cino=>5n-3f0^-2{2: */
/* DiscDB entry FIXME is this needed ? */
struct _Ex_Cddb_Entry
{
   unsigned int entry_id;
   int          entry_genre;
};

/* An entry in the query list  FIXME is this needed ? */
struct query_list_entry 
{
   int  list_genre;
   int  list_id;
   char list_title[256];
   char list_artist[256];
};

/* DiscDB query structure FIXME is this needed ? */
struct _Ex_Cddb_Query
{   
   int    query_match;
   int    query_matches;
   struct query_list_entry query_list[MAX_INEXACT_MATCHES];
};

/* cache functions */
/*******************/

/* See if a disc is in the local database */
int
ex_cddb_disc_cached(Extrackt *ex)
{
   int index,id;
   struct stat st;
   char root_dir[256], file[256];
   
   if(!ex->disc_info.have_info) ex_cddev_stat(ex, TRUE);
   
   id= ex_cddb_discid_get(ex);
   
   snprintf(root_dir, 256, "%s/.cddb", getenv("HOME"));
   
   if(stat(root_dir, &st) < 0)
     return FALSE;
   else 
     {
	if(!S_ISDIR(st.st_mode))
	  return FALSE;
     }
   
   snprintf(file, 256, "%s/%08x", root_dir, id);
   if(stat(file,&st) == 0) return TRUE;
   
   for(index=0; index<12; index++) {
      snprintf(file, 256, "%s/%s/%08x", root_dir, ex_cddb_genre_to_text(index), id);
	
      if(stat(file,&st) == 0)
	return TRUE;
   }
     
   return FALSE;
}

#if 0
/* read from the pipe until the end of message */
int
ex_protocol_receive(Extrackt *ex)
{
   static char *buf = NULL;
   static int size = 1024;
   int i = size - 1024;
   int fd = ex->pfd[0];

   if(!buf)
     buf = calloc(1,size);
   while(i <  size - 1)
      {
	 int n;
	 int nl = 0;
	 n = read(fd, buf + i, 1);
	 if(!n)
	   break;
	 if(n < 0)
	   return 0;
	 if(buf[i-1] == '\n')
	   nl = 1;
	 if(buf[i] == '.' && nl)
	   goto done;
	 if(buf[i] == '\0')
	   {
	      printf("the buffer has %s\n", buf);
	      goto done;
	   }
	 i++;
      }
   /* if we are here is because we have reached the size of the buffer without
    * finding the EOF */
   buf = realloc(buf, size + 1024);
   size += 1024;
   return 0;
done:
   ex_protocol_parse(ex,buf,i);
   buf = NULL;
   i = 0;
   return 1;
}

#endif
/* Read from the local database */
int 
ex_cddb_data_cache_read(Extrackt *ex, Ex_Disc_Data *ddata, const char *encoding)
{
   FILE *discdb_data = NULL;
   int index,genre;
   char root_dir[256],file[256],inbuf[512];
   struct stat st;
   
   snprintf(root_dir, 256, "%s/.cddb", getenv("HOME"));
   
   if(stat(root_dir, &st) < 0) 
     {
	return -1;
     } 
   else 
     {
	if(!S_ISDIR(st.st_mode)) 
	  {
	     errno = ENOTDIR;
	     return -1;
	  }
     }
   
   if(!ex->disc_info.have_info) ex_cddev_stat(ex, TRUE);
   
   ddata->data_id = ex_cddb_discid_get(ex);
   *(ddata->data_extended) = '\0';
   *(ddata->data_title) = '\0';
   *(ddata->data_artist) = '\0';
   *(ddata->data_playlist) = '\0';
   ddata->data_multi_artist = FALSE;
   ddata->data_year = 0;
   ddata->data_genre = 7;
   ddata->data_id3genre = -1;
   ddata->revision = -1;
   
   for(index=0; index< MAX_TRACKS; index++) 
     {
	*(ddata->data_track[index].name) = '\0';
	*(ddata->data_track[index].artist) = '\0';
	*(ddata->data_track[index].extended) = '\0';
	*(ddata->data_track[index].number) = '\0';
     }
   
   snprintf(file, 256, "%s/%08x", root_dir, ddata->data_id);
   if(stat(file, &st) == 0) 
     {
	discdb_data = fopen(file, "r");
     }
   else 
     {
	for(genre=0; genre<12; genre++) 
	  {
	     snprintf(file, 256, "%s/%s/%08x", root_dir, ex_cddb_genre_to_text(genre),
		      ddata->data_id);
	     
	     if(stat(file,&st) == 0) {
		discdb_data = fopen(file, "r");
		
		ddata->data_genre = genre;
		break;
	     }
	  }
	
	if(genre == 12) return -1;
     }
   
   while(fgets(inbuf, 512, discdb_data))
     _ex_cddb_line_process(inbuf, ddata, ex->disc_info.num_tracks);
   
   /* Both disc title and artist have been stuffed in the title field, so they
    * need to be separated */
   
   //ex_cddb_title_parse(ddata->data_title ,ddata->data_title,
//		       ddata->data_artist, "/");
   
   /* FIXME: what do we do with this?
   if(!DiscDBUTF8Validate(disc,ddata)) {
	DiscDBConvertEncoding(disc,ddata,strcasecmp(encoding,"UTF-8")?
			      encoding:"ISO-8859-1","UTF-8");
   }
   */
   fclose(discdb_data);
   
   return 0;
}

/* Write to the local cache */
int 
ex_cddb_cache_write(Extrackt *ex, Ex_Disc_Data *ddata, FILE *outfile,
		    int gripext, int freedbext, char *encoding)
{
   FILE  *discdb_data;
   int    track;
   char   root_dir[256],file[256],tmp[512];
   struct stat st;
   
   if(!ex->disc_info.have_info) ex_cddev_stat(ex, TRUE);
   
   if(!outfile) 
     {
	snprintf(root_dir, 256, "%s/.cddb", getenv("HOME"));
	snprintf(file, 256, "%s/%08x", root_dir, ddata->data_id);
	
	if(stat(root_dir,&st)<0) 
	  {
	     if(errno != ENOENT) 
	       {
		  Debug(_("Stat error %d on %s\n"),errno,root_dir);
		  return -1;
	       }
	     else 
	       {
		  Debug(_("Creating directory %s\n"),root_dir);
		  mkdir(root_dir,0777);
	       }
	  } 
	else 
	  {
	     if(!S_ISDIR(st.st_mode)) 
	       {
		  Debug(_("Error: %s exists, but is a file\n"),root_dir);
		  errno=ENOTDIR;
		  return -1;
	       }
	  }
	    
	if((discdb_data=fopen(file,"w")) == NULL)
	  {
	     Debug(_("Error: Unable to open %s for writing\n"),file);
	     return -1;
	  }
     }
   else 
     discdb_data = outfile;
   
#ifndef GRIPCD
   fprintf(discdb_data,"# xmcd CD database file generated by Grip %s\n",
	   VERSION);
#else
   fprintf(discdb_data,"# xmcd CD database file generated by GCD %s\n",
	   VERSION);
#endif
   fputs("# \n",discdb_data);
   fputs("# Track frame offsets:\n",discdb_data);
   
   for(track = 0; track < ex->disc_info.num_tracks; track++)
     fprintf(discdb_data, "#       %d\n",ex->disc_info.track[track].start_frame);
   
   fputs("# \n", discdb_data);
   fprintf(discdb_data, "# Disc length: %d seconds\n", ex->disc_info.length.mins *
	   60 + ex->disc_info.length.secs);
   fputs("# \n", discdb_data);
   
   if(gripext) 
     fprintf(discdb_data, "# Revision: %d\n", ddata->revision);
   else 
     fprintf(discdb_data, "# Revision: %d\n", ddata->revision + 1);
   
   fprintf(discdb_data, "# Submitted via: Grip %s\n", VERSION);
   fputs("# \n", discdb_data);
   fprintf(discdb_data, "DISCID=%08x\n", ddata->data_id);
   
   snprintf(tmp, 512, "%s / %s", ddata->data_artist, ddata->data_title);
   
   _ex_cddb_line_write("DTITLE", -1, tmp, discdb_data, encoding);
   
   if(gripext||freedbext) 
     {
	if(ddata->data_year)
	  fprintf(discdb_data, "DYEAR=%d\n", ddata->data_year);
	else 
	  fprintf(discdb_data, "DYEAR=\n");
     }
   
   if(gripext) 
     {
	fprintf(discdb_data, "DGENRE=%s\n", ex_cddb_genre_to_text(ddata->data_genre));
	fprintf(discdb_data, "DID3=%d\n", ddata->data_id3genre);
     }
   else if(freedbext) 
     {
	/* FIXME: IMPLEMENT THIS!!!
	fprintf(discdb_data, "DGENRE=%s\n", ID3GenreString(ddata->data_id3genre));
	*/
     }
     
   for(track=0; track < ex->disc_info.num_tracks; track++) 
     {
	if(gripext || !*(ddata->data_track[track].artist)) 
	  {
	     _ex_cddb_line_write("TTITLE", track, ddata->data_track[track].name,
			     discdb_data, encoding);
	  }
	else 
	  {
	     snprintf(tmp, 512, "%s / %s", ddata->data_track[track].artist,
		      ddata->data_track[track].name);
	     _ex_cddb_line_write("TTITLE", track, tmp, discdb_data, encoding);
	  }
	    
	if(gripext && *(ddata->data_track[track].artist))
	  _ex_cddb_line_write("TARTIST", track,
			      ddata->data_track[track].artist,
			      discdb_data,encoding);
     }
     
     _ex_cddb_line_write("EXTD", -1, ddata->data_extended, discdb_data, encoding);
   
   for(track=0; track < ex->disc_info.num_tracks; track++)
     _ex_cddb_line_write("EXTT",track, ddata->data_track[track].extended,
			 discdb_data, encoding);
   
   if(outfile)
     fprintf(discdb_data, "PLAYORDER=\n");
     else 
     {
	fprintf(discdb_data, "PLAYORDER=%s\n", ddata->data_playlist);
	fclose(discdb_data);
     }
     
   return 0;
}

static void 
_ex_cddb_line_write(char *header, int num, char *data, FILE *outfile, char *encoding)
{
   char *offset, *next, *chunk;
   
   /* FIXME: what do we do with this?
   if(strcasecmp(encoding,"utf-8")) {
      StrConvertEncoding(data,"utf-8",encoding,512);
   }
   */  
   offset=data;
   
   do {
      for(next=offset; next-offset < 65 && *next;) 
	{
	   if (*next=='\\' && *(next + 1)) 
	     {
		next+=2;
	     }
	   /* FIXME: what do we do with this?
	   else if(!strcasecmp(encoding, "utf-8")) 
	     {
		next=g_utf8_find_next_char(next,NULL);
	     }
	   */
	   else 
	     {
		next++;
	     }
	}
      
      chunk = ex_util_strndup(offset, (int)(next - offset));
      
      if(num == -1)
	fprintf(outfile, "%s=%s\n", header, chunk);
      else
	fprintf(outfile, "%s%d=%s\n", header, num, chunk);
      free(chunk);
      offset = next;
   } while (*offset);
}


