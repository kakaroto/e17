#include "enlil_private.h"
#include <libexif/exif-data.h>
#include <libexif/exif-ifd.h>
#include <libexif/exif-loader.h>
#include "exif/jpeg-data.h"
#include <libiptcdata/iptc-data.h>
#include <libiptcdata/iptc-jpeg.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/stat.h>
#include <unistd.h>

typedef struct Enlil_Photo_Exif Enlil_Photo_Exif;
typedef struct Enlil_Photo_IPTC Enlil_Photo_IPTC;

struct Enlil_Photo_Exif
{
   //list of Enlil_Exif *
   Eina_List *l;
};

struct Enlil_Photo_IPTC
{
   //list of Enlil_IPTC *
   Eina_List *l;
};

struct enlil_photo
{
   Enlil_Album *album;

   Enlil_Photo_Type type;

   //some Photo object must never be saved
   Eina_Bool mustNotBeSaved: 1;

   const char *name;
   const char *file_name;
   const char *path;
   const char *description;
   long long time;
   const char *thumb_fdo_large;
   const char *thumb_fdo_normal;
   Eina_Bool eet_save;

   int size;
   int size_w;
   int size_h;

   struct
     {
		int id;
		int timestamp_last_update_header;
		int timestamp_last_update_tags;
		int timestamp_last_update_file;
     } netsync;

   Eina_Bool exif_loaded;
   Enlil_Photo_Exif *exifs;
   //Exifs data saved here because an access is required
   //without loading all the data.
   //For example DateTimeOriginal is used to sort the list of photo
   struct
     {
	const char *DateTimeOriginal;
	double longitude;
	double latitude;
     } exif_data;

   Eina_Bool iptc_loaded;
   Enlil_Photo_IPTC *iptcs;

   //list of Enlil_Photo_Tags *
   Eina_List *tags;

   void *user_data;
   Enlil_Photo_Free_Cb free_cb;
};

#define PHOTO_EXIF_LOAD(photo) \
   if(!photo->exifs) \
{ \
   _photo_eet_exifs_load((Enlil_Photo*)photo); \
}

#define PHOTO_IPTC_LOAD(photo) \
   if(!photo->iptcs) \
{ \
   _photo_eet_iptcs_load((Enlil_Photo*)photo); \
}

static Enlil_Photo_Exif *_photo_eet_exifs_load(Enlil_Photo *photo);
static Eet_Data_Descriptor * enlil_photo_exif_edd_new(Eet_Data_Descriptor *edd_exif);
static int _photo_eet_exif_save(Enlil_Photo *photo);

static Enlil_Photo_IPTC *_photo_eet_iptcs_load(Enlil_Photo *photo);
static Eet_Data_Descriptor * enlil_photo_iptc_edd_new(Eet_Data_Descriptor *edd_iptc);
static int _photo_eet_iptc_save(Enlil_Photo *photo);

Enlil_Photo *enlil_photo_new()
{
   Enlil_Photo *photo = calloc(1,sizeof(Enlil_Photo));
   photo->eet_save = 1;
   photo->exif_data.longitude = 360.;
   photo->exif_data.latitude = 360.;
   return photo;
}

Enlil_Photo *enlil_photo_copy_new(Enlil_Photo *photo)
{
   ASSERT_RETURN(photo!=NULL);

   Enlil_Photo *_photo = enlil_photo_new();

   enlil_photo_copy(photo, _photo);
   return _photo;
}

/**
 * @brief Copy photo_src in photo_dest. This method can be used when a change occurs in a photo file. We have a photo (photo_src) which is updated and a second photo (photo_dest) which is not updated.
 */
void enlil_photo_copy(Enlil_Photo *photo_src, Enlil_Photo *photo_dest)
{
   Enlil_Photo_Tag *photo_tag, *photo_tag2;;
   const Eina_List *l, *l2;
   Eina_List *tags;
   Eina_Bool find;
   ASSERT_RETURN_VOID(photo_src!=NULL);
   ASSERT_RETURN_VOID(photo_dest!=NULL);

   photo_dest->eet_save = photo_src->eet_save;
   photo_dest->type = photo_src->type;
   enlil_photo_name_set(photo_dest, enlil_photo_name_get(photo_src));
   enlil_photo_file_name_set(photo_dest, enlil_photo_file_name_get(photo_src));
   enlil_photo_path_set(photo_dest, enlil_photo_path_get(photo_src));
   enlil_photo_time_set(photo_dest, enlil_photo_time_get(photo_src));
   enlil_photo_thumb_fdo_normal_set(photo_dest, enlil_photo_thumb_fdo_normal_get(photo_src));
   enlil_photo_thumb_fdo_large_set(photo_dest, enlil_photo_thumb_fdo_large_get(photo_src));
   enlil_photo_size_set(photo_dest, enlil_photo_size_get(photo_src));
   enlil_photo_size_w_set(photo_dest, enlil_photo_size_w_get(photo_src));
   enlil_photo_size_h_set(photo_dest, enlil_photo_size_h_get(photo_src));


   tags = eina_list_clone(enlil_photo_tags_get(photo_dest));

   EINA_LIST_FOREACH(enlil_photo_tags_get(photo_src), l, photo_tag)
     {
	find = EINA_FALSE;
	EINA_LIST_FOREACH(tags, l2, photo_tag2)
	  {
	     if(photo_tag2->name == photo_tag->name)
	       {
		  find = EINA_TRUE;
		  tags = eina_list_remove(tags, photo_tag2);
		  break;
	       }
	  }
	if(!find)
	  {
	     Enlil_Photo_Tag *_photo_tag = calloc(1, sizeof(Enlil_Photo_Tag));
	     _photo_tag->name = eina_stringshare_add(photo_tag->name);
	     _photo_tag->photo = photo_dest;
	     _photo_tag->tag = NULL;
	     photo_dest->tags = eina_list_append(photo_dest->tags, _photo_tag);
	  }
     }
   EINA_LIST_FREE(tags, photo_tag)
     {
	enlil_photo_tag_remove(photo_dest, photo_tag);
     }
}

void enlil_photo_free(Enlil_Photo **photo)
{
   Enlil_Photo_Tag *photo_tag;
   Enlil_Exif *exif;
   Enlil_IPTC *iptc;
   ASSERT_RETURN_VOID(photo!=NULL);
   ASSERT_RETURN_VOID((*photo)!=NULL);

   EINA_LIST_FREE( (*photo)->tags, photo_tag)
     {
	if((*photo)->album && enlil_album_library_get((*photo)->album))
	  _enlil_library_tag_photo_remove(enlil_album_library_get((*photo)->album), photo_tag, *photo);

	eina_stringshare_del(photo_tag->name);
	free(photo_tag);
     }

   if( (*photo)->exifs )
     {
	EINA_LIST_FREE( (*photo)->exifs->l, exif)
	  {
	     enlil_exif_free(&exif);
	  }
	FREE( (*photo)->exifs );
     }

   if( (*photo)->iptcs )
     {
	EINA_LIST_FREE( (*photo)->iptcs->l, iptc)
	  {
	     enlil_iptc_free(&iptc);
	  }
	FREE( (*photo)->iptcs );
     }

   if( (*photo)->free_cb )
     (*photo)->free_cb((*photo), (*photo)->user_data);

   EINA_STRINGSHARE_DEL((*photo)->name);
   EINA_STRINGSHARE_DEL((*photo)->file_name);
   EINA_STRINGSHARE_DEL((*photo)->path);

   FREE(*photo);
}

void enlil_photo_list_print(const Eina_List *l_photos)
{
   const Eina_List *l;
   const Enlil_Photo *photo;
   ASSERT_RETURN_VOID(l_photos!=NULL);
   EINA_LIST_FOREACH(l_photos, l, photo)
     {
	enlil_photo_print(photo);
     }
}

void enlil_photo_print(const Enlil_Photo *photo)
{
   ASSERT_RETURN_VOID(photo!=NULL);
   printf("# Photo %s\n", photo->name);
   printf("\tFile name\t:\t%s\n", photo->file_name);
   printf("\tpath\t\t:\t%s\n", photo->path);
}

void enlil_photo_album_set(Enlil_Photo *photo, Enlil_Album *album)
{
   ASSERT_RETURN_VOID(photo != NULL);
   photo->album = album;
}

#define FCT_NAME enlil_photo
#define STRUCT_TYPE Enlil_Photo

   SET(type, Enlil_Photo_Type)
   SET(mustNotBeSaved, Eina_Bool)
   STRING_SET(file_name)
   SET(time, long long)
   SET(eet_save, Eina_Bool)
   SET(exif_loaded, Eina_Bool)
   SET(iptc_loaded, Eina_Bool)
   SET(size, int)
   SET(size_w, int)
SET(size_h, int)


   GET(type, Enlil_Photo_Type)
   GET(mustNotBeSaved, Eina_Bool)
   GET(album, Enlil_Album*)
   GET(name, const char*)
   GET(path, const char*)
   GET(file_name, const char*)
   GET(time, long long)
   GET(user_data, void *)
   GET(thumb_fdo_normal, const char*)
   GET(description, const char *)
   GET(thumb_fdo_large, const char*)
   GET(tags, const Eina_List *)
   GET(eet_save, Eina_Bool)
   GET(exif_loaded, Eina_Bool)
   GET(iptc_loaded, Eina_Bool)
   GET(size, int)
   GET(size_w, int)
GET(size_h, int)

#undef FCT_NAME
#undef STRUCT_TYPE


int enlil_photo_netsync_id_get(Enlil_Photo *photo)
{
    ASSERT_RETURN(photo!=NULL);

    return photo->netsync.id;
}


int enlil_photo_netsync_timestamp_last_update_header_get(Enlil_Photo *photo)
{
    ASSERT_RETURN(photo!=NULL);

    return photo->netsync.timestamp_last_update_header;
}

void _enlil_photo_netsync_timestamp_last_update_header_set(Enlil_Photo *photo, int timestamp)
{
    ASSERT_RETURN_VOID(photo!=NULL);

    photo->netsync.timestamp_last_update_header = timestamp;

    enlil_photo_eet_save(photo);
    Enlil_Album *album = enlil_photo_album_get(photo);
    if(album)
 	   enlil_album_eet_photos_list_save(album);
}

int enlil_photo_netsync_timestamp_last_update_tags_get(Enlil_Photo *photo)
{
    ASSERT_RETURN(photo!=NULL);

    return photo->netsync.timestamp_last_update_tags;
}

void _enlil_photo_netsync_timestamp_last_update_tags_set(Enlil_Photo *photo, int timestamp)
{
    ASSERT_RETURN_VOID(photo!=NULL);

    photo->netsync.timestamp_last_update_tags = timestamp;

    enlil_photo_eet_save(photo);
    Enlil_Album *album = enlil_photo_album_get(photo);
    if(album)
 	   enlil_album_eet_photos_list_save(album);
}
int enlil_photo_netsync_timestamp_last_update_file_get(Enlil_Photo *photo)
{
    ASSERT_RETURN(photo!=NULL);

    return photo->netsync.timestamp_last_update_file;
}

void _enlil_photo_netsync_timestamp_last_update_file_set(Enlil_Photo *photo, int timestamp)
{
    ASSERT_RETURN_VOID(photo!=NULL);

    photo->netsync.timestamp_last_update_file = timestamp;

    enlil_photo_eet_save(photo);
    Enlil_Album *album = enlil_photo_album_get(photo);
    if(album)
 	   enlil_album_eet_photos_list_save(album);
}

void _enlil_photo_netsync_id_set(Enlil_Photo *photo, int id)
{
   ASSERT_RETURN_VOID(photo != NULL);

   photo->netsync.id = id;
}


void enlil_photo_name_set(Enlil_Photo *photo, const char *name)
{
   ASSERT_RETURN_VOID(photo!=NULL);
   ASSERT_RETURN_VOID(name != NULL);

   if(name[0] == '\0' && photo->name == NULL)
   	   return ;

  const char *new_name = eina_stringshare_add(name);

  if(photo->name == new_name)
  {
   EINA_STRINGSHARE_DEL(new_name);
   return ;
  }
  EINA_STRINGSHARE_DEL(photo->name);

  photo->name = new_name;

   if(photo->album && enlil_album_photos_sort_get(photo->album) == ENLIL_PHOTO_SORT_NAME)
     _enlil_album_photo_name_changed(photo->album, photo);

   enlil_photo_iptc_add(photo, "ObjectName", photo->name, EINA_FALSE);

   _enlil_photo_netsync_timestamp_last_update_header_set(photo, time(NULL));
}

void enlil_photo_description_set(Enlil_Photo *photo, const char *desc)
{
   ASSERT_RETURN_VOID(photo!=NULL);
   ASSERT_RETURN_VOID(desc != NULL);

   if(desc[0] == '\0' && photo->description == NULL)
	   return ;

   const char *new_description = eina_stringshare_add(desc);

   if(photo->description == new_description)
   {
	   EINA_STRINGSHARE_DEL(new_description);
	   return ;
   }
   EINA_STRINGSHARE_DEL(photo->description);

   photo->description = new_description;

   enlil_photo_iptc_add(photo, "Caption", photo->description, EINA_FALSE);

   _enlil_photo_netsync_timestamp_last_update_header_set(photo, time(NULL));
}

void enlil_photo_exif_clear(Enlil_Photo *photo)
{
   Enlil_Exif *exif;

   ASSERT_RETURN_VOID(photo!=NULL);

   if( photo->exifs )
     {
	EINA_LIST_FREE( photo->exifs->l, exif)
	  {
	     enlil_exif_free(&exif);
	  }
	FREE(photo->exifs);
     }
}

void enlil_photo_iptc_clear(Enlil_Photo *photo)
{
   Enlil_IPTC *iptc;

   ASSERT_RETURN_VOID(photo!=NULL);

   if( photo->iptcs )
     {
	EINA_LIST_FREE( photo->iptcs->l, iptc)
	  {
	     enlil_iptc_free(&iptc);
	  }
	FREE(photo->iptcs);
     }
}

/**
 * Get the longitude where the photo was taken. Returns 360 if the longitude is unknown
 */
double enlil_photo_longitude_get(const Enlil_Photo *photo)
{
   ASSERT_RETURN(photo!=NULL);

   return photo->exif_data.longitude;
}

/**
 * Get the latitude where the photo was taken. Returns 360 if the latitude is unknown
 */
double enlil_photo_latitude_get(const Enlil_Photo *photo)
{
   ASSERT_RETURN(photo!=NULL);

   return photo->exif_data.latitude;
}


const Eina_List *enlil_photo_exifs_get(const Enlil_Photo *photo)
{
   ASSERT_RETURN(photo!=NULL);

   PHOTO_EXIF_LOAD(photo);
   return photo->exifs->l;
}

const Eina_List *enlil_photo_iptcs_get(const Enlil_Photo *photo)
{
   ASSERT_RETURN(photo!=NULL);

   PHOTO_IPTC_LOAD(photo);
   return photo->iptcs->l;
}

/**
 * tag must be in eina_stringshare
 */
Enlil_Exif *enlil_photo_exif_get_from_tag(const Enlil_Photo *photo, const char *tag)
{
   const Eina_List *l;
   Enlil_Exif *exif;

   ASSERT_RETURN(photo != NULL);
   ASSERT_RETURN(tag != NULL);

   enlil_photo_exifs_get(photo);

   EINA_LIST_FOREACH(enlil_photo_exifs_get(photo), l, exif)
     {
	if(enlil_exif_tag_get(exif) == tag)
	  return exif;
     }

   return NULL;
}

/*
 * name and value must be in eina_stringshare
 */
Enlil_IPTC *enlil_photo_iptc_get_from_name_and_value(const Enlil_Photo *photo, const char *name, const char *value)
{
   const Eina_List *l;
   Enlil_IPTC *iptc;

   ASSERT_RETURN(photo != NULL);
   ASSERT_RETURN(name != NULL);

   enlil_photo_exifs_get(photo);

   EINA_LIST_FOREACH(enlil_photo_iptcs_get(photo), l, iptc)
     {
	if(enlil_iptc_name_get(iptc) == name &&
	      (!value || enlil_iptc_value_get(iptc) == value))
	  return iptc;
     }

   return NULL;
}

void enlil_photo_copy_exif_in_file(Enlil_Photo *photo, const char *file)
{
   char buf[PATH_MAX];
   JPEGData *data;
   ExifData *ed;

   ASSERT_RETURN_VOID(photo != NULL);
   ASSERT_RETURN_VOID(file != NULL);

   snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(photo),
	 enlil_photo_file_name_get(photo));

   if(!enlil_photo_jpg_is(file) || !enlil_photo_jpg_is(buf))
     {
	LOG_INFO("Exif data can only be copied from a jpg file into a jpg file");
	return ;
     }

   ed = exif_data_new_from_file(buf);
   data = jpeg_data_new_from_file (file);
   jpeg_data_set_exif_data (data, ed);
   jpeg_data_save_file(data, file);
}

void enlil_photo_save_iptc_in_file(Enlil_Photo *photo)
{
   char buf[PATH_MAX];

   ASSERT_RETURN_VOID(photo != NULL);
   snprintf(buf, PATH_MAX, "%s/%s", enlil_photo_path_get(photo), enlil_photo_file_name_get(photo));
   enlil_photo_save_iptc_in_custom_file(photo, buf);
}


void enlil_photo_save_iptc_in_custom_file(Enlil_Photo *photo, const char *file)
{
   IptcData * d;
   IptcDataSet * ds;
   IptcRecord record;
   IptcTag tag;
   Enlil_IPTC *iptc;
   Eina_List *l;
   FILE *infile, *outfile;
   unsigned char * iptc_buf = NULL;
   unsigned int iptc_len;
   int ps3_len;
   unsigned char outbuf[256*256];
   char tmpfile[PATH_MAX];
   int v;

   ASSERT_RETURN_VOID(photo != NULL);
   ASSERT_RETURN_VOID(file != NULL);

   if(!photo->iptcs) return ;

   if(!enlil_photo_jpg_is(file))
     {
	LOG_INFO("Iptc data can only be saved into a jpg file");
	return ;
     }

   d = iptc_data_new();

   EINA_LIST_FOREACH(photo->iptcs->l, l, iptc)
     {
	ds = iptc_dataset_new();

	if (iptc_tag_find_by_name (enlil_iptc_name_get(iptc), &record, &tag) < 0) {
	     LOG_ERR("Invalid tag name : #%s#", enlil_iptc_name_get(iptc));
	     continue ;
	}

	ds = iptc_dataset_new();
	iptc_dataset_set_tag(ds, record, tag);

	const IptcTagInfo* info = iptc_tag_get_info(record, tag);

	switch (info->format)
	  {
	   case IPTC_FORMAT_BYTE:
	   case IPTC_FORMAT_SHORT:
	   case IPTC_FORMAT_LONG:
	      iptc_dataset_set_value(ds, atol(enlil_iptc_value_get(iptc)), IPTC_DONT_VALIDATE);
	      break;
	   default:
	      iptc_dataset_set_data(ds, (unsigned char *)enlil_iptc_value_get(iptc), strlen(enlil_iptc_value_get(iptc)), IPTC_DONT_VALIDATE);
	      break;
	  }

	iptc_data_add_dataset(d, ds);
	iptc_dataset_unref(ds);
     }


   iptc_data_sort (d);

   if (iptc_data_save (d, &iptc_buf, &iptc_len) < 0) {
	LOG_ERR("Failed to generate IPTC bytestream");
	return ;
   }
   ps3_len = iptc_jpeg_ps3_save_iptc (NULL, 0,
	 iptc_buf, iptc_len, outbuf, sizeof(outbuf));
   iptc_data_free_buf (d, iptc_buf);
   if (ps3_len < 0) {
	LOG_ERR("Failed to generate PS3 header");
	return ;
   }

   infile = fopen (file, "r");
   if (!infile) {
	LOG_ERR("Can't reopen input file");
	return ;
   }
   snprintf (tmpfile, PATH_MAX, "%s.%d", file, getpid());
   outfile = fopen (tmpfile, "w");
   if (!outfile) {
	LOG_ERR("Can't open temporary file for writing");
	return ;
   }

   v = iptc_jpeg_save_with_ps3 (infile, outfile, outbuf, ps3_len);
   fclose (infile);
   fclose (outfile);
   if (v < 0) {
	unlink (tmpfile);
	LOG_ERR("Failed to save image");
	return ;
   }

   if (rename (tmpfile, file) < 0) {
	LOG_ERR("Failed to save image");
	unlink (tmpfile);
	return ;
   }
}



void enlil_photo_exifs_set(Enlil_Photo *photo, Eina_List *exifs)
{
   Enlil_Exif *exif;
   Enlil_Exif *exif_datetimeoriginal = NULL;
   Enlil_Exif *exif_datetimeoriginal_new = NULL;
   const char *s = eina_stringshare_add("DateTimeOriginal");
   const char *slong = eina_stringshare_add("GPSLongitude");
   const char *slong_ref = eina_stringshare_add("GPSLongitudeRef");

   const char *slat = eina_stringshare_add("GPSLatitude");
   const char *slat_ref = eina_stringshare_add("GPSLatitudeRef");

   //becouse for me the latitude is there :/
   const char *slat2 = eina_stringshare_add("InteroperabilityVersion");
   const char *slat2_ref = eina_stringshare_add("InteroperabilityIndex");


   ASSERT_RETURN_VOID(photo != NULL);

   if(photo->exifs)
     {
	EINA_LIST_FREE(photo->exifs->l, exif)
	  {
	     if(enlil_exif_tag_get(exif) == s)
	       exif_datetimeoriginal = exif;
	     else
	       enlil_exif_free(&exif);
	  }
     }
   else
     photo->exifs = calloc(1, sizeof(Enlil_Photo_Exif)) ;

   photo->exifs->l = exifs;

   //longitude
   exif = enlil_photo_exif_get_from_tag(photo, slong_ref);
   if(exif)
     {
	if( strcmp(enlil_exif_value_get(exif), "E"))
	  photo->exif_data.longitude *= -1;
     }
   if(exif)
     {
	exif = enlil_photo_exif_get_from_tag(photo, slong);
	if(exif)
	  {
	     float deg, min, sec;
	     sscanf(enlil_exif_value_get(exif), "%f, %f, %f", &deg, &min, &sec);
	     photo->exif_data.longitude = deg + min/60 + sec/3600;
	  }
	else
	  photo->exif_data.longitude = 360;
     }
   else
     photo->exif_data.longitude = 360;
   //


   //latitude
   exif = enlil_photo_exif_get_from_tag(photo, slat_ref);
   if(exif)
     {
	if(strcmp(enlil_exif_value_get(exif), "N"))
	  photo->exif_data.latitude *= -1;
     }
   else
     {
	exif = enlil_photo_exif_get_from_tag(photo, slat2_ref);
	if(exif)
	  {
	     if(!strcmp(enlil_exif_value_get(exif), "S"))
	       photo->exif_data.latitude *= -1;
	     else if(strcmp(enlil_exif_value_get(exif), "N"))
	       exif = NULL;
	  }
     }

   if(exif)
     {
	exif = enlil_photo_exif_get_from_tag(photo, slat);
	if(exif)
	  {
	     float deg, min, sec;
	     sscanf(enlil_exif_value_get(exif), "%f, %f, %f", &deg, &min, &sec);
	     photo->exif_data.latitude = deg + min/60 + sec/3600;
	  }
	else
	  {
	     exif = enlil_photo_exif_get_from_tag(photo, slat2);
	     if(exif)
	       {
		  float deg, min, sec;
		  sscanf(enlil_exif_value_get(exif), "%f, %f, %f", &deg, &min, &sec);
		  photo->exif_data.latitude = deg + min/60 + sec/3600;
	       }
	     else
	       photo->exif_data.latitude = 360;
	  }
     }
   else
     photo->exif_data.latitude = 360;
   //

   exif_datetimeoriginal_new = enlil_photo_exif_get_from_tag(photo, s);
   EINA_STRINGSHARE_DEL(photo->exif_data.DateTimeOriginal);
   if(exif_datetimeoriginal_new)
     {
	photo->exif_data.DateTimeOriginal = eina_stringshare_add(enlil_exif_value_get(exif_datetimeoriginal_new));
	enlil_photo_eet_save(photo);
     }

   if(photo->album)
     {
	if(!exif_datetimeoriginal || !exif_datetimeoriginal_new
	      || enlil_exif_value_get(exif_datetimeoriginal) != enlil_exif_value_get(exif_datetimeoriginal_new) )
	  {
	     _enlil_album_photo_datetimeoriginal_changed(photo->album, photo);
	  }
     }
   _photo_eet_exif_save(photo);
   EINA_STRINGSHARE_DEL(s);
   if(exif_datetimeoriginal)
     enlil_exif_free(&exif_datetimeoriginal);
}

void enlil_photo_iptcs_set(Enlil_Photo *photo, Eina_List *iptcs)
{
   Enlil_IPTC *iptc;
   Eina_List *l, *l2;
   IptcRecord iptcrecord;
   IptcTag iptctag;
   Enlil_Photo_Tag *tag;
   Eina_List *tags;
   Eina_Bool find;

   ASSERT_RETURN_VOID(photo != NULL);

   if(photo->iptcs)
     {
	EINA_LIST_FREE(photo->iptcs->l, iptc)
	   enlil_iptc_free(&iptc);
     }
   else
     photo->iptcs = calloc(1, sizeof(Enlil_Photo_Exif)) ;

   photo->iptcs->l = iptcs;


   //load the list of tags from the iptc data (name = Keywords)
   iptc_tag_find_by_name ("Keywords", &iptcrecord, &iptctag);

   tags = eina_list_clone(photo->tags);
   EINA_LIST_FOREACH(photo->iptcs->l, l, iptc)
     {
	if(enlil_iptc_record_get(iptc) != iptcrecord
	      || enlil_iptc_tag_get(iptc) != iptctag)
	  continue;

	find = EINA_FALSE;
	EINA_LIST_FOREACH(tags, l2, tag)
	  {
	     if(tag->name == enlil_iptc_value_get(iptc))
	       {
		  tags = eina_list_remove(tags, tag);
		  find = EINA_TRUE;
		  break;
	       }
	  }
	if(!find)
	  enlil_photo_tag_add(photo, enlil_iptc_value_get(iptc));
     }

   EINA_LIST_FREE(tags, tag)
     {
	enlil_photo_tag_remove(photo, tag);
     }

   //load the description
   iptc_tag_find_by_name ("Caption", &iptcrecord, &iptctag);

   EINA_LIST_FOREACH(photo->iptcs->l, l, iptc)
     {
	if(enlil_iptc_record_get(iptc) != iptcrecord
	      || enlil_iptc_tag_get(iptc) != iptctag)
	  continue;

	EINA_STRINGSHARE_DEL(photo->description);
	photo->description = eina_stringshare_add(enlil_iptc_value_get(iptc));
	break;
     }

   //load the name
   iptc_tag_find_by_name ("ObjectName", &iptcrecord, &iptctag);

   EINA_LIST_FOREACH(photo->iptcs->l, l, iptc)
     {
	if(enlil_iptc_record_get(iptc) != iptcrecord
	      || enlil_iptc_tag_get(iptc) != iptctag)
	  continue;

	EINA_STRINGSHARE_DEL(photo->name);
	photo->name = eina_stringshare_add(enlil_iptc_value_get(iptc));
	break;
     }

   enlil_photo_eet_save(photo);
   _photo_eet_iptc_save(photo);
}

void enlil_photo_user_data_set(Enlil_Photo *photo, void *user_data, Enlil_Photo_Free_Cb cb)
{
   ASSERT_RETURN_VOID(photo!=NULL);
   photo->user_data = user_data;
   photo->free_cb = cb;
}

void enlil_photo_path_set(Enlil_Photo *photo, const char *path)
{
   ASSERT_RETURN_VOID(photo!=NULL);
   ASSERT_RETURN_VOID(path!=NULL);

   EINA_STRINGSHARE_DEL(photo->path);

   char *_path = strdup(path);
   int len = strlen(_path) -1;
   if(_path[len] == '/')
     _path[len] = '\0';

   photo->path = eina_stringshare_add(_path);
   FREE(_path);
}

void enlil_photo_thumb_fdo_normal_set(Enlil_Photo *photo,const char *thumb)
{
   ASSERT_RETURN_VOID(photo != NULL);

   photo->thumb_fdo_normal = eina_stringshare_add(thumb);

   enlil_photo_eet_save(photo);
}


void enlil_photo_thumb_fdo_large_set(Enlil_Photo *photo,const char *thumb)
{
   ASSERT_RETURN_VOID(photo != NULL);

   photo->thumb_fdo_large = eina_stringshare_add(thumb);

   enlil_photo_eet_save(photo);
}

int enlil_photo_is(const char *file)
{
   ASSERT_RETURN(file!=NULL);

   char *strip_ext = ecore_file_strip_ext(file);
   const char *ext = file + strlen(strip_ext);
   FREE(strip_ext);

   if(ext && (strcmp(ext,".jpeg") == 0
	    || strcmp(ext, ".jpg") == 0
	    || strcmp(ext, ".JPG") == 0
	    || strcmp(ext, ".JPEG") == 0
	    || strcmp(ext, ".png") ==0
	    || strcmp(ext, ".PNG") == 0) )
     return 1;

   return 0;
}

int enlil_video_is(const char *file)
{
   ASSERT_RETURN(file!=NULL);

   char *strip_ext = ecore_file_strip_ext(file);
   const char *ext = file + strlen(strip_ext);
   FREE(strip_ext);

   if(ext && (strcmp(ext,".mpeg") == 0
	    || strcmp(ext, ".MPEG") == 0
	    || strcmp(ext, ".avi") == 0
	    || strcmp(ext, ".AVI") == 0
	    || strcmp(ext, ".ogv") == 0
	    || strcmp(ext, ".OGV") == 0
	    || strcmp(ext, ".mkv") == 0
	    || strcmp(ext, ".MKV") == 0) )
     return 1;

   return 0;
}

int enlil_photo_jpg_is(const char *file)
{
   ASSERT_RETURN(file!=NULL);

   char *strip_ext = ecore_file_strip_ext(file);
   const char *ext = file + strlen(strip_ext);
   FREE(strip_ext);

   if(ext && (strcmp(ext,".jpeg") == 0
	    || strcmp(ext, ".jpg") == 0
	    || strcmp(ext, ".JPG") == 0
	    || strcmp(ext, ".JPEG") == 0 ) )
     return 1;

   return 0;
}

/**
 * If the photo is in an album which is in a library ( see enlil_photo_album_set() and enlil_album_library_set()), this method will add the photo
 * in the tag defined in the library. If the tag does not exists, it is created.
 *
 * @param photo The photo struct
 */
void enlil_photo_tag_process(Enlil_Photo *photo)
{
   Eina_List *l;
   Enlil_Photo_Tag *photo_tag;
   ASSERT_RETURN_VOID(photo != NULL);
   ASSERT_RETURN_VOID(photo->album != NULL);
   ASSERT_RETURN_VOID(enlil_album_library_get(photo->album) != NULL);

   EINA_LIST_FOREACH(photo->tags, l, photo_tag)
     {
	photo_tag->photo = photo;
	if(!photo_tag -> tag)
	  _enlil_library_tag_photo_add(enlil_album_library_get(photo->album), photo_tag, photo);
     }
}

/**
 * Add the a tag to the photo .
 *
 * If the photo is in an album which is in a library  ( see enlil_photo_album_set() and enlil_album_library_set()), this method will add the photo
 * in the tag defined in the library. If the tag does not exists, it is created.
 *
 * @param photo The photo struct
 * @param tag_name The name of the tag
 */
void enlil_photo_tag_add(Enlil_Photo *photo, const char *tag_name)
{
   Enlil_Photo_Tag *photo_tag;
   Eina_List *l;
   ASSERT_RETURN_VOID(photo != NULL);
   ASSERT_RETURN_VOID(tag_name != NULL);

   tag_name = eina_stringshare_add(tag_name);

   EINA_LIST_FOREACH(photo->tags, l, photo_tag)
     {
	if(photo_tag->name == tag_name)
	  goto end;
     }

   photo_tag = calloc(1, sizeof(Enlil_Photo_Tag));
   photo_tag->name = eina_stringshare_add(tag_name);
   photo_tag->photo = photo;

   photo->tags = eina_list_append(photo->tags, photo_tag);

   if(photo->album && enlil_album_library_get(photo->album))
     {
	Enlil_Library *library = enlil_album_library_get(photo->album);
	_enlil_library_tag_photo_add(library, photo_tag, photo);
	enlil_library_eet_tags_save(library);
     }

   if(enlil_photo_iptc_add(photo, "Keywords", photo_tag->name, EINA_TRUE))
	enlil_photo_save_iptc_in_file(photo);

   enlil_photo_eet_save(photo);

end:
   eina_stringshare_del(tag_name);
}

Eina_Bool enlil_photo_iptc_add(Enlil_Photo *photo, const char *name, const char *value, Eina_Bool check_value)
{
   Enlil_IPTC *iptc, *iptc1;

   ASSERT_RETURN(photo != NULL);
   ASSERT_RETURN(name != NULL);

   iptc = enlil_iptc_new_from_name(name);
   if(check_value)
     iptc1 = enlil_photo_iptc_get_from_name_and_value(photo, enlil_iptc_name_get(iptc), value);
   else
     iptc1 = enlil_photo_iptc_get_from_name_and_value(photo, enlil_iptc_name_get(iptc), NULL);

   if(!iptc1)
     {
	enlil_iptc_value_set(iptc, eina_stringshare_add(value));
	photo->iptcs->l = eina_list_append(photo->iptcs->l, iptc);
	_photo_eet_iptc_save(photo);

	return EINA_TRUE;
     }
   else if(!check_value)
     {
	enlil_iptc_value_set(iptc1, value);
	enlil_iptc_free(&iptc);
	return EINA_TRUE;
     }
   else
     enlil_iptc_free(&iptc);

   return EINA_FALSE;
}

/**
 * Remove the photo from the tag.
 *
 * If the photo is in an album which is in a library ( see enlil_photo_album_set() and enlil_album_library_set()), this method will remove the photo
 * from the tag defined in the library.
 *
 * @param photo The photo struct
 * @param photo_tag The photo tag struct
 */
void enlil_photo_tag_remove(Enlil_Photo *photo, Enlil_Photo_Tag *photo_tag)
{
   ASSERT_RETURN_VOID(photo != NULL);
   ASSERT_RETURN_VOID(photo_tag != NULL);

   photo->tags = eina_list_remove(photo->tags, photo_tag);

   if(photo->album && enlil_album_library_get(photo->album)) 
     {
	Enlil_Library *library = enlil_album_library_get(photo->album);
	_enlil_library_tag_photo_remove(library, photo_tag, photo);
	enlil_library_eet_tags_save(library);
     }

   if(enlil_photo_iptc_remove(photo, "Keywords", photo_tag->name))
     enlil_photo_save_iptc_in_file(photo);

   eina_stringshare_del(photo_tag->name);
   free(photo_tag);

   enlil_photo_eet_save(photo);
}

Eina_Bool enlil_photo_iptc_remove(Enlil_Photo *photo, const char *name, const char *value)
{
   Enlil_IPTC *iptc;

   ASSERT_RETURN(photo != NULL);
   ASSERT_RETURN(name != NULL);

   name = eina_stringshare_add(name);
   iptc = enlil_photo_iptc_get_from_name_and_value(photo, name, value);
   eina_stringshare_del(name);

   if(iptc)
     {
	photo->iptcs->l = eina_list_remove(photo->iptcs->l, iptc);
	enlil_iptc_free(&iptc);
	_photo_eet_iptc_save(photo);
	return EINA_TRUE;
     }
   return EINA_FALSE;
}


/**
 * @brief Load a photo from the file "file" or "file_path"
 * @param file An Eet file handler. If Null the file "file_path" will be open and closed
 * @param file_path An Eet file path. If "file" is NULL, the photo will be loaded from the file.
 * @param photo_key The Eet photo key.
 * @return Returns the photo or NULL if failed
 */
Enlil_Photo * enlil_photo_eet_load(const char *eet_path, const char *key)
{
   Eet_Data_Descriptor *edd, *edd_tag;
   Eet_File *f ;
   Enlil_Photo *data;
   char buf[PATH_MAX];

   ASSERT_RETURN(key!=NULL);
   ASSERT_RETURN(eet_path!=NULL);

   if(key[0]!='/')
     snprintf(buf, PATH_MAX, "/photo %s", key);
   else
     strncpy(buf, key, PATH_MAX);

   f = enlil_file_manager_open(eet_path);
   ASSERT_RETURN(f!=NULL);

   edd_tag = _enlil_photo_tag_edd_new();
   edd = enlil_photo_edd_new(edd_tag);

   data = eet_data_read(f, edd, buf);

   enlil_file_manager_close(eet_path);

   eet_data_descriptor_free(edd_tag);
   eet_data_descriptor_free(edd);

   if(data)
     data->eet_save = 1;
   return data;
}


int enlil_photo_eet_remove(const char *eet_path, const char* key)
{
   Eet_File *f;
   char buf[PATH_MAX];

   ASSERT_RETURN(eet_path!=NULL);
   ASSERT_RETURN(key!=NULL);

   if(key[0]!='/')
     snprintf(buf, PATH_MAX, "/photo %s", key);
   else
     strncpy(buf, key, PATH_MAX);

   f = enlil_file_manager_open(eet_path);
   ASSERT_RETURN(f!=NULL);

   eet_delete(f, buf);
   enlil_file_manager_close(eet_path);

   return 1;
}


int enlil_photo_eet_save(Enlil_Photo *photo)
{
   Eet_Data_Descriptor *edd, *edd_tag;
   Eet_File *f;
   char buf[PATH_MAX], path[PATH_MAX];
   int res;

   ASSERT_RETURN(photo!=NULL);

   if(photo->mustNotBeSaved)
	   return 1;

   if(!photo->eet_save) return 0;

   snprintf(path, PATH_MAX, "%s/"EET_FILE, enlil_photo_path_get(photo));
   f = enlil_file_manager_open(path);

   ASSERT_RETURN(f!=NULL);

   edd_tag = _enlil_photo_tag_edd_new();
   edd = enlil_photo_edd_new(edd_tag);

   snprintf(buf, PATH_MAX, "/photo %s", enlil_photo_file_name_get(photo));
   res=eet_data_write(f, edd, buf, photo, 0);

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_tag);
   eet_data_descriptor_free(edd);

   return res;
}


/**
 * Return the DateTimeOriginal tag retrieve from the exifs data without
 * loading the exifs data;
 */
const char *_enlil_photo_exif_datetimeoriginal_get(const Enlil_Photo *photo)
{
   ASSERT_RETURN(photo != NULL);

   return photo->exif_data.DateTimeOriginal;
}

static Enlil_Photo_Exif *_photo_eet_exifs_load(Enlil_Photo *photo)
{
   Eet_Data_Descriptor *edd, *edd_exif;
   Eet_File *f ;
   Enlil_Photo_Exif *data;
   char buf[PATH_MAX];
   char path[PATH_MAX];

   ASSERT_RETURN(photo!=NULL);

   snprintf(buf, PATH_MAX, "/exif %s", enlil_photo_file_name_get(photo));
   snprintf(path, PATH_MAX, "%s/"EET_FILE, enlil_photo_path_get(photo));

   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f!=NULL);

   edd_exif = _enlil_exif_edd_new();
   edd = enlil_photo_exif_edd_new(edd_exif);

   data = eet_data_read(f, edd, buf);

   enlil_file_manager_close(path);

   eet_data_descriptor_free(edd_exif);
   eet_data_descriptor_free(edd);

   if(data)
     photo->exifs = data;
   else
     photo->exifs = calloc(1, sizeof(Enlil_Photo_Exif));

   return data;
}

static int _photo_eet_exif_save(Enlil_Photo *photo)
{
   Eet_Data_Descriptor *edd, *edd_exif;
   Eet_File *f;
   char buf[PATH_MAX], path[PATH_MAX];
   int res;

   ASSERT_RETURN(photo!=NULL);

   if(photo->mustNotBeSaved)
	   return 1;

   if(!photo->eet_save) return 0;

   snprintf(path, PATH_MAX, "%s/"EET_FILE, enlil_photo_path_get(photo));
   f = enlil_file_manager_open(path);

   ASSERT_RETURN(f!=NULL);

   edd_exif = _enlil_exif_edd_new();
   edd = enlil_photo_exif_edd_new(edd_exif);

   snprintf(buf, PATH_MAX, "/exif %s", enlil_photo_file_name_get(photo));
   res=eet_data_write(f, edd, buf, photo->exifs, 0);

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_exif);
   eet_data_descriptor_free(edd);

   return res;
}

static Enlil_Photo_IPTC *_photo_eet_iptcs_load(Enlil_Photo *photo)
{
   Eet_Data_Descriptor *edd, *edd_iptc;
   Eet_File *f ;
   Enlil_Photo_IPTC *data;
   char buf[PATH_MAX];
   char path[PATH_MAX];

   ASSERT_RETURN(photo!=NULL);

   snprintf(buf, PATH_MAX, "/iptc %s", enlil_photo_file_name_get(photo));
   snprintf(path, PATH_MAX, "%s/"EET_FILE, enlil_photo_path_get(photo));

   f = enlil_file_manager_open(path);
   ASSERT_RETURN(f!=NULL);

   edd_iptc = _enlil_iptc_edd_new();
   edd = enlil_photo_iptc_edd_new(edd_iptc);

   data = eet_data_read(f, edd, buf);

   enlil_file_manager_close(path);

   eet_data_descriptor_free(edd_iptc);
   eet_data_descriptor_free(edd);

   if(data)
     photo->iptcs = data;
   else
     photo->iptcs = calloc(1, sizeof(Enlil_Photo_IPTC));

   return data;
}

static int _photo_eet_iptc_save(Enlil_Photo *photo)
{
   Eet_Data_Descriptor *edd, *edd_iptc;
   Eet_File *f;
   char buf[PATH_MAX], path[PATH_MAX];
   int res;

   ASSERT_RETURN(photo!=NULL);
   if(photo->mustNotBeSaved)
	   return 1;

   if(!photo->eet_save) return 0;

   snprintf(path, PATH_MAX, "%s/"EET_FILE, enlil_photo_path_get(photo));
   f = enlil_file_manager_open(path);

   ASSERT_RETURN(f!=NULL);

   edd_iptc = _enlil_iptc_edd_new();
   edd = enlil_photo_iptc_edd_new(edd_iptc);

   snprintf(buf, PATH_MAX, "/iptc %s", enlil_photo_file_name_get(photo));
   res=eet_data_write(f, edd, buf, photo->iptcs, 0);

   enlil_file_manager_close(path);
   eet_data_descriptor_free(edd_iptc);
   eet_data_descriptor_free(edd);

   return res;
}

Eet_Data_Descriptor * _enlil_photo_file_name_edd_new()
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;


   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Photo);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "path", path, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "file_name", file_name, EET_T_STRING);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "netsync.id", netsync.id, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "netsync.timestamp_last_update_header", netsync.timestamp_last_update_header, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "netsync.timestamp_last_update_tags", netsync.timestamp_last_update_tags, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "netsync.timestamp_last_update_file", netsync.timestamp_last_update_file, EET_T_INT);

   return edd;
}


Eet_Data_Descriptor * enlil_photo_edd_new(Eet_Data_Descriptor *edd_tag)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   ASSERT_RETURN(edd_tag != NULL);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Photo);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "type", type, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "name", name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "path", path, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "file_name", file_name, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "description", description, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "time", time, EET_T_LONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "thumb_fdo_large", thumb_fdo_large, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "thumb_fdo_normal", thumb_fdo_normal, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "size", size, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "size_w", size_w, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "size_h", size_h, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "exif_loaded", exif_loaded, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "iptc_loaded", iptc_loaded, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "exif_DateTimeOriginal", exif_data.DateTimeOriginal, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "exif_longitude", exif_data.longitude, EET_T_DOUBLE);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "exif_latitude", exif_data.latitude, EET_T_DOUBLE);

   EET_DATA_DESCRIPTOR_ADD_LIST(edd, Enlil_Photo, "tags", tags, edd_tag);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "netsync.id", netsync.id, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "netsync.timestamp_last_update_header", netsync.timestamp_last_update_header, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "netsync.timestamp_last_update_tags", netsync.timestamp_last_update_tags, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo, "netsync.timestamp_last_update_file", netsync.timestamp_last_update_file, EET_T_INT);

   return edd;
}


static Eet_Data_Descriptor * enlil_photo_exif_edd_new(Eet_Data_Descriptor *edd_exif)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   ASSERT_RETURN(edd_exif != NULL);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Photo_Exif);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_LIST(edd, Enlil_Photo_Exif, "exifs", l, edd_exif);

   return edd;
}

static Eet_Data_Descriptor * enlil_photo_iptc_edd_new(Eet_Data_Descriptor *edd_iptc)
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   ASSERT_RETURN(edd_iptc != NULL);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Photo_IPTC);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_LIST(edd, Enlil_Photo_Exif, "iptcs", l, edd_iptc);

   return edd;
}


Eet_Data_Descriptor * _enlil_photo_tag_edd_new()
{
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor_Class eddc;

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Enlil_Photo_Tag);
   eddc.func.str_direct_alloc = NULL;
   eddc.func.str_direct_free = NULL;

   edd = eet_data_descriptor_file_new(&eddc);

   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Enlil_Photo_Tag, "name", name, EET_T_STRING);

   return edd;
}

