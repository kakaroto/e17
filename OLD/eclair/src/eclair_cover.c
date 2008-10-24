#include "eclair_cover.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <Ecore_File.h>
#include "eclair.h"
#include "eclair_playlist.h"
#include "eclair_media_file.h"
#include "eclair_utils.h"

#define PACKET_CHUNK_SIZE 1024
#define MAX_REQUEST_SIZE 1024
#define NUM_EXTENSIONS 3
#define NUM_COVER_NAMES 2

typedef struct _Eclair_Cover_Packet_Chunk Eclair_Cover_Packet_Chunk;
typedef struct _Eclair_Cover_Not_In_DB_Album Eclair_Cover_Not_In_DB_Album;

struct _Eclair_Cover_Packet_Chunk
{
   int size;
   char *data;
};

struct _Eclair_Cover_Not_In_DB_Album
{
   char *artist;
   char *album;
};

const char *amazon_hostname = "webservices.amazon.com";
const char *amazon_license_key = "0P1862RFDFSF4KYZQNG2";
const char *cover_extensions[NUM_EXTENSIONS] = { "jpg", "jpeg", "png" };
const char *cover_names[NUM_COVER_NAMES] = { "cover", "folder" };

static int _eclair_cover_connect_to_hostname(const char *hostname);
static int _eclair_cover_connect_to_host(const struct hostent *host);
static Evas_Bool _eclair_cover_receive_next_packet(int socket_fd, char **packet, int *length);
static int _eclair_cover_extract_http_body(char *packet, int packet_length, char **body);
static int _eclair_cover_fetch(const char *url, char **data);

static char *_eclair_cover_build_path_from_filepath(Eclair_Cover_Manager *cover_manager, char *file_path, const char *cover_extension);
static char *_eclair_cover_build_path_from_artist_album(Eclair_Cover_Manager *cover_manager, const char *artist, const char *album, const char *cover_extension);
static char *_eclair_cover_build_cover_filename(const char *cover_name, const char *cover_extension);
static void _eclair_cover_build_url_for_item_search(const char *keywords, char url[MAX_REQUEST_SIZE]);
static void _eclair_cover_build_url_for_item_images(const char *ASIN, char url[MAX_REQUEST_SIZE]);
static xmlNode *_eclair_cover_get_node_xml_tree(xmlNode *root_node, const char *prop);
static xmlChar *_eclair_cover_get_prop_value_from_xml_tree(xmlNode *root_node, const char *prop);
static void _eclair_cover_add_file_to_not_in_amazon_db_list(Eina_List **list, const char *artist, const char *album);

static void *_eclair_cover_thread(void *param);

//Initialize the cover module
void eclair_cover_init(Eclair_Cover_Manager *cover_manager, Eclair *eclair)
{
   if (!cover_manager)
      return;

   cover_manager->cover_add_state = ECLAIR_IDLE;
   cover_manager->cover_files_to_add = NULL;
   cover_manager->cover_files_to_treat = NULL;
   cover_manager->not_in_amazon_db = NULL;
   cover_manager->eclair = eclair;
   cover_manager->cover_should_treat_files = 0;
   cover_manager->cover_delete_thread = 0;
   pthread_cond_init(&cover_manager->cover_cond, NULL);
   pthread_mutex_init(&cover_manager->cover_mutex, NULL);
   pthread_create(&cover_manager->cover_thread, NULL, _eclair_cover_thread, cover_manager);
}

//Shutdown the cover module
void eclair_cover_shutdown(Eclair_Cover_Manager *cover_manager)
{
   Eina_List *l;
   Eclair_Cover_Not_In_DB_Album *album;

   if (!cover_manager)
      return;

   for (l = cover_manager->not_in_amazon_db; l; l = l->next)
   {
      if (!(album = l->data))
         continue;
      free(album->artist);
      free(album->album);
      free(album);
   }
   eina_list_free(cover_manager->not_in_amazon_db);

   fprintf(stderr, "Cover: Debug: Destroying cover thread\n");
   cover_manager->cover_delete_thread = 1;
   pthread_cond_broadcast(&cover_manager->cover_cond);
   pthread_join(cover_manager->cover_thread, NULL);
   fprintf(stderr, "Cover: Debug: Cover thread destroyed\n");
}

//Add a media file to the list of files to treat
void eclair_cover_add_file_to_treat(Eclair_Cover_Manager *cover_manager, Eclair_Media_File *media_file)
{
   if (!cover_manager || !media_file || cover_manager->cover_delete_thread)
      return;

   media_file->in_cover_process = 1;
   while (cover_manager->cover_add_state != ECLAIR_IDLE)
      usleep(10000);
   cover_manager->cover_add_state = ECLAIR_ADDING_FILE_TO_ADD;
   cover_manager->cover_files_to_add = eina_list_append(cover_manager->cover_files_to_add, media_file);
   cover_manager->cover_add_state = ECLAIR_IDLE;
   
   cover_manager->cover_should_treat_files = 1;
   pthread_cond_broadcast(&cover_manager->cover_cond); 
}

//Fetch from amazon the covers of the files stored in the list cover_files_to_treat
static void *_eclair_cover_thread(void *param)
{
   Eclair_Cover_Manager *cover_manager;
   Eclair *eclair;
   Eina_List *l, *next;
   Eclair_Media_File *current_file;

   if (!(cover_manager = param) || !(eclair = cover_manager->eclair))
      return NULL;

   pthread_mutex_lock(&cover_manager->cover_mutex);
   for (;;)
   {
      if (cover_manager->cover_should_treat_files || cover_manager->cover_delete_thread)
         cover_manager->cover_should_treat_files = 0;
      else
         pthread_cond_wait(&cover_manager->cover_cond, &cover_manager->cover_mutex);
      while (cover_manager->cover_files_to_treat || cover_manager->cover_files_to_add || cover_manager->cover_delete_thread)
      {
         if (cover_manager->cover_delete_thread)
         {
            cover_manager->cover_files_to_treat = eina_list_free(cover_manager->cover_files_to_treat);
            cover_manager->cover_files_to_add = eina_list_free(cover_manager->cover_files_to_add);
            cover_manager->cover_delete_thread = 0;
            return NULL;
         }
         //Add the new files to the list of files to treat
         if (cover_manager->cover_files_to_add)
         {
            while (cover_manager->cover_add_state != ECLAIR_IDLE)
               usleep(10000);
            cover_manager->cover_add_state = ECLAIR_ADDING_FILE_TO_TREAT;
            for (l = cover_manager->cover_files_to_add; l; l = l->next)
               cover_manager->cover_files_to_treat = eina_list_append(cover_manager->cover_files_to_treat, l->data);
            cover_manager->cover_files_to_add = eina_list_free(cover_manager->cover_files_to_add);
            cover_manager->cover_add_state = ECLAIR_IDLE;
         }
         //Treat the files in the list
         for (l = cover_manager->cover_files_to_treat; l || cover_manager->cover_delete_thread; l = next)
         {
            if (cover_manager->cover_delete_thread || cover_manager->cover_files_to_add)
               break;
            next = l->next;
            current_file = l->data;
            cover_manager->cover_files_to_treat = eina_list_remove_list(cover_manager->cover_files_to_treat, l);
            if (current_file)
            {
               if (current_file->delete_me)
               {
                  current_file->in_cover_process = 0;
                  continue;
               }
               current_file->cover_path = eclair_cover_file_get(cover_manager, current_file->artist, current_file->album, current_file->path);
               eclair_media_file_update(eclair, current_file);
               current_file->in_cover_process = 0;
            }
         }
      }
   }
   return NULL;
}

//Return the path of the file of the album cover
//NULL if failed. The returned path will have to be freed if success
char *eclair_cover_file_get(Eclair_Cover_Manager *cover_manager, const char *artist, const char *album, char *file_path)
{
   char *path;

   if (!cover_manager)
      return NULL;

   if ((path = eclair_cover_file_get_from_local(cover_manager, artist, album, file_path)))
      return path;   
   if ((path = eclair_cover_file_get_from_amazon(cover_manager, artist, album)))
      return path;
   return NULL;
}

//Try to open the cover file on the disk if it exists
//NULL if failed. The returned path will have to be freed if success
char *eclair_cover_file_get_from_local(Eclair_Cover_Manager *cover_manager, const char *artist, const char *album, char *file_path)
{
   char *cover_filename, *cover_path;
   int i, j;

   for (i = 0; i < NUM_EXTENSIONS; i++)
   {
      if ((cover_path = _eclair_cover_build_path_from_filepath(cover_manager, file_path, cover_extensions[i]))
         && ecore_file_exists(cover_path))
         return cover_path;
      free(cover_path);
   }

   for (i = 0; i < NUM_EXTENSIONS; i++)
   {
      if ((cover_path = _eclair_cover_build_path_from_artist_album(cover_manager, artist, album, cover_extensions[i]))
         && ecore_file_exists(cover_path))
         return cover_path;
      free(cover_path);
   }

   for (i = 0; i < NUM_COVER_NAMES; i++)
   {
      for (j = 0; j < NUM_EXTENSIONS; j++)
      {
         if ((cover_filename = _eclair_cover_build_cover_filename(cover_names[i], cover_extensions[j]))
            && (cover_path = eclair_utils_search_file(cover_filename, ecore_file_dir_get(file_path))))
         {
            free(cover_filename);
            return cover_path;
         }
         free(cover_filename);
      }
   }

   return NULL;
}

//Fetch the cover corresponding to artist and album, and return the path of the file
//NULL if failed. The returned path will have to be freed if success
char *eclair_cover_file_get_from_amazon(Eclair_Cover_Manager *cover_manager, const char *artist, const char *album)
{
   char *body, *cover_path;
   int body_length;
   xmlDocPtr doc;
   xmlNode *image_node;
   xmlChar *ASIN, *cover_url;
   char url[MAX_REQUEST_SIZE];
   char *keywords, *converted_keywords;
   FILE *cover_file;
   Eina_List *l;
   Eclair_Cover_Not_In_DB_Album *not_in_db_album;

   if (!cover_manager || !artist || !album || (strlen(artist) <= 0) || (strlen(album) <= 0))
      return NULL;

   //Check if we already perform a search on this album and if amazon answered it doesn't have this album in database
   for (l = cover_manager->not_in_amazon_db; l; l = l->next)
   {
      if (!(not_in_db_album = l->data))
         continue;
      if (!not_in_db_album->artist || !not_in_db_album->album)
         continue;
      if (strcmp(not_in_db_album->artist, artist) == 0 && strcmp(not_in_db_album->album, album) == 0)
         return NULL;
   }

   //Get the ASIN of the album
   keywords = malloc(strlen(artist) + strlen(album) + 2);
   sprintf(keywords, "%s %s", artist, album);
   converted_keywords = eclair_utils_add_uri_special_chars(keywords);
   free(keywords);

   _eclair_cover_build_url_for_item_search(converted_keywords, url);
   free(converted_keywords);
   if (cover_manager->cover_delete_thread)
      return NULL;
   body_length = _eclair_cover_fetch(url, &body);
   if (body_length <= 0)
   {
      fprintf(stderr, "Cover: Unable to download cover from amazon.com\n");
      return NULL;
   }

   doc = xmlReadMemory(body, body_length, "noname.xml", NULL, 0);
   free(body);
   if (!doc)
   {
      fprintf(stderr, "Cover: Amazon.com webservices sent a non xml response\n");
      return NULL;
   }
   ASIN = _eclair_cover_get_prop_value_from_xml_tree(xmlDocGetRootElement(doc), "ASIN");
   xmlFreeDoc(doc);
   if (!ASIN)
   {
      fprintf(stderr, "Cover: Unable to find the item \"%s - %s\" on amazon\n", artist, album);
      _eclair_cover_add_file_to_not_in_amazon_db_list(&cover_manager->not_in_amazon_db, artist, album);
      return NULL;
   }


   //Get cover url from the ASIN
   _eclair_cover_build_url_for_item_images(ASIN, url);
   xmlFree(ASIN);
   if (cover_manager->cover_delete_thread)
      return NULL;
   body_length = _eclair_cover_fetch(url, &body);
   if (body_length <= 0)
   {
      fprintf(stderr, "Cover: Unable to download cover from amazon.com\n");
      return NULL;
   }

   doc = xmlReadMemory(body, body_length, "noname.xml", NULL, 0);
   free(body);
   if (!doc)
   {
      fprintf(stderr, "Cover: Amazon.com webservices sent a non xml response\n");
      return NULL;
   }
   if (!(image_node = _eclair_cover_get_node_xml_tree(xmlDocGetRootElement(doc), "LargeImage")))
   {
      if (!(image_node = _eclair_cover_get_node_xml_tree(xmlDocGetRootElement(doc), "MediumImage")))
         image_node = _eclair_cover_get_node_xml_tree(xmlDocGetRootElement(doc), "SmallImage");
   }
   cover_url = _eclair_cover_get_prop_value_from_xml_tree(image_node, "URL");
   xmlFreeDoc(doc);
   if (!cover_url)
   {
      fprintf(stderr, "Cover: Unable to find the cover for %s - %s\n", artist, album);
      _eclair_cover_add_file_to_not_in_amazon_db_list(&cover_manager->not_in_amazon_db, artist, album);
      return NULL;
   }


   //Fetch the cover
   if (cover_manager->cover_delete_thread)
   {
      xmlFree(cover_url);
      return NULL;
   }
   body_length = _eclair_cover_fetch(cover_url, &body);
   xmlFree(cover_url);
   if (body_length <= 0)
   {
      fprintf(stderr, "Cover: Unable to download cover from amazon.com\n");
      return NULL;
   }

   //Save the file and return the path
   cover_path = _eclair_cover_build_path_from_artist_album(cover_manager, artist, album, "jpg");
   cover_file = fopen(cover_path, "wb");
   if (!cover_file)
   {
      fprintf(stderr, "Cover: Unable to open the file \"%s\" to save the cover\n", cover_path);
      free(cover_path);
      free(body);
      return NULL;
   }
   fwrite(body, body_length, sizeof(char), cover_file);
   free(body);
   fclose(cover_file);

   return cover_path;
}

//Try to connect to an hostname
//Return the file descriptor of the socket created (need to be destroyed with close())
//-1 if failed
static int _eclair_cover_connect_to_hostname(const char *hostname)
{
   struct hostent *he;

   if (!hostname)
      return 0;

   if (!(he = gethostbyname(hostname)))
   {
      fprintf(stderr, "Cover: Unable to find IP of %s\n", hostname);
      return -1;
   }
   return _eclair_cover_connect_to_host(he);
}

//Try to connect to an host
//Return the file descriptor of the socket created (need to be destroyed with close())
//-1 if failed
static int _eclair_cover_connect_to_host(const struct hostent *host)
{
   int socket_fd;
   struct sockaddr_in adress;

   if (!host)
      return 0;

   if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      fprintf(stderr, "Cover: Unable to create an Internet socket\n");
      return 0;
   }

   bzero(&adress, sizeof(struct sockaddr_in));
   adress.sin_family = AF_INET;
   adress.sin_port = htons(80);
   memcpy(&adress.sin_addr, host->h_addr, host->h_length);
   if (connect(socket_fd, (struct sockaddr *)&adress, sizeof(struct sockaddr)) != 0)
   {
      fprintf(stderr, "Cover: Unable to connect to %s\n", inet_ntoa(adress.sin_addr));
      close(socket_fd);
      return -1;
   }
   
   return socket_fd;
}

//Try to fetch the file stored at url on host he (may be NULL) and put the data into data
//data has to be freed if success
//Return the length of the data (negative if failed) 
static int _eclair_cover_fetch(const char *url, char **data)
{
   const char *host_start;
   const char *url_abs_path;
   char request[MAX_REQUEST_SIZE];
   char *host, *packet, *body;
   int packet_length, body_length;
   int socket_fd;

   if (!url || !data)
      return -1;

   if (strncmp(url, "http://", 7) != 0)
   {
      fprintf(stderr, "Cover: Invalid URL\n");
      return -1;
   }
   host_start = url + 7;
   url_abs_path = index(host_start, '/');
   if (!url_abs_path)
   {
      fprintf(stderr, "Cover: Invalid URL\n");
      return -1;
   }

   host = malloc(url_abs_path - host_start + 1);
   strncpy(host, host_start, url_abs_path - host_start);
   host[url_abs_path - host_start] = 0;
   if ((socket_fd = _eclair_cover_connect_to_hostname(host)) < 0)
   {
      free(host);
      return -1;
   }
   
   sprintf(request, "GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", url_abs_path, host);
   if (send(socket_fd, request, strlen(request), 0) <= 0)
   {
      fprintf(stderr, "Cover: Unable to send request to host: %s\n", host);
      free(host);
      close(socket_fd);
      return -1;
   }
   _eclair_cover_receive_next_packet(socket_fd, &packet, &packet_length);
   body_length = _eclair_cover_extract_http_body(packet, packet_length, &body);
   close(socket_fd);
   free(host);
   if (body_length <= 0)
   {
      fprintf(stderr, "Cover: Unable to fetch the file stored at %s\n", url);
      free(packet);
      return body_length;
   }

   *data = malloc(body_length);
   memcpy(*data, body, body_length);
   free(packet);
   return body_length;
}

//Get the packet sent by the context and its length
//Return 0 if failed. The packet will have to freed by the caller
static Evas_Bool _eclair_cover_receive_next_packet(int socket_fd, char **packet, int *length)
{
   char packet_chunk[PACKET_CHUNK_SIZE];
   int num_bytes_read, pos;
   Eclair_Cover_Packet_Chunk *chunk;
   Eina_List *l, *chunks = NULL;

   if (socket_fd < 0 || !packet || !length)
      return 0;

   *length = 0;
   while ((num_bytes_read = read(socket_fd, packet_chunk, PACKET_CHUNK_SIZE)) > 0)
   {
      chunk = malloc(sizeof(Eclair_Cover_Packet_Chunk));
      chunk->size = num_bytes_read;
      chunk->data = malloc(num_bytes_read);
      memcpy(chunk->data, packet_chunk, num_bytes_read);
      chunks = eina_list_append(chunks, chunk);
      *length += num_bytes_read;
   }

   if (*length <= 0)
   {
      *length = 0;
      *packet = NULL;
      return 0;
   }

   pos = 0;
   *packet = malloc(*length);
   for (l = chunks; l; l = l->next)
   {
      if (!(chunk = l->data))
         continue;
      memcpy(*packet + pos, chunk->data, chunk->size);
      free(chunk->data);
      pos += chunk->size;
      free(chunk);
   }
   eina_list_free(chunks);

   return 1;
}

//Get the body of the http packet and return its length
//Returned value is negative if failed
static int _eclair_cover_extract_http_body(char *packet, int packet_length, char **body)
{
   char *body_start;
   int body_length, response_code, http_version;

   if (!packet || packet_length <= 0)
      return -1;

   if (sscanf(packet, "HTTP/1.%d %d", &http_version, &response_code) != 2)
   {
      fprintf(stderr, "Cover: Invalid HTTP response\n");
      return -1;
   }
   if (response_code != 200)
   {
      fprintf(stderr, "Cover: Invalid response code: %d\n", response_code);
      return -1;
   }

   if (!(body_start = strstr(packet, "\r\n\r\n")))
   {
      fprintf(stderr, "Cover: Invalid HTTP response\n");
      return -1;
   }
   body_start += 4;
   *body = body_start;
   body_length = packet_length - (body_start - packet);
   return body_length;
}

//Build the cover file path from file_path and cover_extension
//The returned path has to be freed
static char *_eclair_cover_build_path_from_filepath(Eclair_Cover_Manager *cover_manager, char *file_path, const char *cover_extension)
{
   char *cover_path, *filename_without_ext;

   if (!cover_manager || !file_path || !cover_extension || !cover_manager->eclair || strlen(file_path) <= 0)
      return NULL;

   filename_without_ext = eclair_utils_file_get_filename_without_ext(file_path);
   cover_path = malloc(strlen(cover_manager->eclair->config.covers_dir_path) + strlen(filename_without_ext) + strlen(cover_extension) + 2);
   sprintf(cover_path, "%s%s.%s", cover_manager->eclair->config.covers_dir_path, filename_without_ext, cover_extension);
   free(filename_without_ext);

   return cover_path;
}

//Build the cover file path from artist and album
//The returned path has to be freed
static char *_eclair_cover_build_path_from_artist_album(Eclair_Cover_Manager *cover_manager, const char *artist, const char *album, const char *cover_extension)
{
   char *filename, *path, *c;

   if (!cover_manager || !artist || !album || !cover_extension || !cover_manager->eclair
      || strlen(artist) <= 0 || strlen(album) <= 0)
      return NULL; 

   filename = malloc(strlen(artist) + strlen(album) + strlen(cover_extension) + 3);
   sprintf(filename, "%s_%s.%s", artist, album, cover_extension);
   //Remove forbidden '/' in the filename
   for (c = index(filename, '/'); c; c = index(c, '/'))
      *c = '_';

   path = malloc(strlen(cover_manager->eclair->config.covers_dir_path) + strlen(filename) + 1);
   sprintf(path, "%s%s", cover_manager->eclair->config.covers_dir_path, filename);
   free(filename);

   return path;
}

//Build the cover filename from the cover name ("cover", "folder", ...) and its extension
//The returned filename has to be freed
static char *_eclair_cover_build_cover_filename(const char *cover_name, const char *cover_extension)
{
   char *cover_filename;

   if (!cover_name || !cover_extension)
      return NULL;

   cover_filename = malloc(strlen(cover_name) + strlen(cover_extension) + 2);
   sprintf(cover_filename, "%s.%s", cover_name, cover_extension);

   return cover_filename;
}

//Build a request to make a search on amazon.com webservices
static void _eclair_cover_build_url_for_item_search(const char *keywords, char url[MAX_REQUEST_SIZE])
{
   sprintf(url, "http://%s/onca/xml?Service=AWSECommerceService&SubscriptionId=%s&Operation=ItemSearch&Keywords=%s&SearchIndex=Music",
      amazon_hostname, amazon_license_key, keywords);
}

//Build an request to get images of the item corresponding to the ASIN
static void _eclair_cover_build_url_for_item_images(const char *ASIN, char url[MAX_REQUEST_SIZE])
{
   sprintf(url, "http://%s/onca/xml?Service=AWSECommerceService&SubscriptionId=%s&Operation=ItemLookup&ItemId=%s&ResponseGroup=Images",
      amazon_hostname, amazon_license_key, ASIN);
}

//Search recursively the node called prop in the xml tree and return its value
static xmlNode *_eclair_cover_get_node_xml_tree(xmlNode *root_node, const char *prop)
{
   xmlNode *n, *children_node;

   for (n = root_node; n; n = n->next)
   {
      if (n->type == XML_ELEMENT_NODE && xmlStrcmp(prop, n->name) == 0)
         return n;
      if ((children_node = _eclair_cover_get_node_xml_tree(n->children, prop)))
         return children_node;
   }

   return NULL;
}

//Search the node recursively called prop in the xml tree and return its value
static xmlChar *_eclair_cover_get_prop_value_from_xml_tree(xmlNode *root_node, const char *prop)
{
   xmlNode *node;

   if (!(node = _eclair_cover_get_node_xml_tree(root_node, prop)))
      return NULL;
   else return xmlNodeGetContent(node);
}

//Add the album to the list of albums which amazon doesn't have in its database
static void _eclair_cover_add_file_to_not_in_amazon_db_list(Eina_List **list, const char *artist, const char *album)
{
   Eclair_Cover_Not_In_DB_Album *new_album;
   
   if (!artist || !album || !list)
      return;

   new_album = malloc(sizeof(Eclair_Cover_Not_In_DB_Album));
   new_album->artist = strdup(artist);
   new_album->album = strdup(album);
   *list = eina_list_prepend(*list, new_album);
}

//Set the cover of the file
void eclair_cover_current_set(Eclair_Cover_Manager *cover_manager, const char *uri)
{
   char *clean_uri, *cover_path, *new_path;
   Eclair_Media_File *current_file;

   if (!cover_manager || !cover_manager->eclair || !uri
      || !(current_file = eclair_playlist_current_media_file(&cover_manager->eclair->playlist)))
      return;

   if (eclair_utils_uri_is_mrl(uri))
   {
      if (!(clean_uri = eclair_utils_remove_uri_special_chars(uri)))
         return;
   
      if (strlen(uri) <= 7 || strncmp(uri, "file://", 7) != 0)
      {
         free(clean_uri);
         return;
      }
      cover_path = strdup(&clean_uri[7]);
      free(clean_uri);
   }
   else
      cover_path = strdup(uri);

   if (!(new_path = _eclair_cover_build_path_from_artist_album(cover_manager, current_file->artist, current_file->album, eclair_utils_file_get_extension(cover_path)))
      && !(new_path = _eclair_cover_build_path_from_filepath(cover_manager, current_file->path, eclair_utils_file_get_extension(cover_path))))
   {
      free(cover_path);
      return;
   }
   if (!ecore_file_cp(cover_path, new_path))
   {
      free(cover_path);
      free(new_path);
      return;
   }

   free(cover_path);
   free(current_file->cover_path);
   current_file->cover_path = new_path;
   eclair_update_current_file_info(cover_manager->eclair, 1);
}
