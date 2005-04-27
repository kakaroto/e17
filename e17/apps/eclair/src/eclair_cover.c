#include "eclair_cover.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/nanohttp.h>

#define PACKET_CHUNK_SIZE 1024
#define MAX_URL_SIZE 1000

const char *amazon_hostname = "webservices.amazon.com";
const char *amazon_license_key = "0P1862RFDFSF4KYZQNG2";

static char *_eclair_cover_build_cover_path(Eclair *eclair, const char *artist, const char *album);
static void _eclair_cover_build_url_for_search(const char *keywords, char url[MAX_URL_SIZE]);
static void _eclair_cover_build_url_for_item_images(const char *ASIN, char url[MAX_URL_SIZE]);
static char *_eclair_cover_convert_to_url_format(const char *string);
static Evas_Bool _eclair_cover_receive_next_packet(void *context, char **packet, int *length);
static xmlNode *_eclair_cover_get_node_xml_tree(xmlNode *root_node, const char *prop);
static xmlChar *_eclair_cover_get_prop_value_from_xml_tree(xmlNode *root_node, const char *prop);

//Initialize the cover module
void eclair_cover_init()
{
   LIBXML_TEST_VERSION
   xmlNanoHTTPInit();
}

//Shutdown the cover module
void eclair_cover_shutdown()
{
   xmlNanoHTTPCleanup();
   xmlCleanupParser();
   xmlMemoryDump();
}

//Return the path of the file of the album cover
//NULL if failed. The returned path will have to be freed if success
char *eclair_cover_file_get(Eclair *eclair, const char *artist, const char *album)
{
   char *path;

   if (!eclair)
      return NULL;

   if ((path = eclair_cover_file_get_from_local(eclair, artist, album)))
      return path;
   if ((path = eclair_cover_file_get_from_amazon(eclair, artist, album)))
      return path;
   return NULL;
}

//Try to open the cover file on the disk if it exists
//NULL if failed. The returned path will have to be freed if success
char *eclair_cover_file_get_from_local(Eclair *eclair, const char *artist, const char *album)
{
   char *cover_path;
   FILE *cover_file;

   if (!eclair)
      return NULL;

   cover_path = _eclair_cover_build_cover_path(eclair, artist, album);
   //TODO: Better way to check if a file exists?!?
   if ((cover_file = fopen(cover_path, "rt")))
   {
      fclose(cover_file);
      return cover_path;
   }

   return NULL;
}

//Fetch the cover corresponding to artist and album, and return the path of the file
//NULL if failed. The returned path will have to be freed if success
char *eclair_cover_file_get_from_amazon(Eclair *eclair, const char *artist, const char *album)
{
   void *context;
   char *packet, *cover, *cover_path;
   int packet_length, keywords_length, cover_size;
   xmlDocPtr doc;
   xmlNode *image_node;
   xmlChar *ASIN, *cover_url;
   char url[MAX_URL_SIZE];
   char *keywords, *converted_keywords;
   FILE *cover_file;

   if (!artist || !album || !eclair)
      return NULL;

   //Get the ASIN of the album
   keywords_length = strlen(artist) + strlen(album) + 2;
   keywords = (char *)malloc(keywords_length);
   sprintf(keywords, "%s %s", artist, album);
   keywords[keywords_length] = 0;
   converted_keywords = _eclair_cover_convert_to_url_format(keywords);
   free(keywords);
   _eclair_cover_build_url_for_search(converted_keywords, url);
   free(converted_keywords);

   context = xmlNanoHTTPOpen(url, NULL);
   if (!context)
   {
      printf("Unable to connect to amazon.com webservices\n");
      return NULL;
   }
   _eclair_cover_receive_next_packet(context, &packet, &packet_length);
   xmlNanoHTTPClose(context);
   if (!packet)
   {
      printf("Unable to get response from amazon.com webservices\n");
      return NULL;
   }

   doc = xmlReadMemory(packet, packet_length, "noname.xml", NULL, 0);
   free(packet);
   if (!doc)
   {
      printf("Amazon.com webservices sent a non xml response\n");
      return NULL;
   }

   ASIN = _eclair_cover_get_prop_value_from_xml_tree(xmlDocGetRootElement(doc), "ASIN");
   xmlFreeDoc(doc);
   if (!ASIN)
   {
      printf("Unable to find the item \"%s - %s\" on amazon\n", artist, album);
      return NULL;
   }


   //Get cover url from the ASIN
   _eclair_cover_build_url_for_item_images((char *)ASIN, url);
   xmlFree(ASIN);

   context = xmlNanoHTTPOpen(url, NULL);
   if (!context)
   {
      printf("Unable to connect to amazon.com webservices\n");
      return NULL;
   }
   _eclair_cover_receive_next_packet(context, &packet, &packet_length);
   xmlNanoHTTPClose(context);
   if (!packet)
   {
      printf("Unable to get response from amazon.com webservices\n");
      return NULL;
   }

   doc = xmlReadMemory(packet, packet_length, "noname.xml", NULL, 0);
   free(packet);
   if (!doc)
   {
      printf("Amazon.com webservices sent a non xml response\n");
      return NULL;
   }

   image_node = _eclair_cover_get_node_xml_tree(xmlDocGetRootElement(doc), "LargeImage");
   cover_url = _eclair_cover_get_prop_value_from_xml_tree(image_node, "URL");
   xmlFreeDoc(doc);
   if (!cover_url)
   {
      printf("Unable to find the cover for %s - %s\n", artist, album);
      return NULL;
   }

   //Fetch the cover
   context = xmlNanoHTTPOpen(cover_url, NULL);
   xmlFree(cover_url);
   if (!context)
   {
      printf("Unable to connect to amazon.com to fetch the cover\n");
      return NULL;
   }
   _eclair_cover_receive_next_packet(context, &cover, &cover_size);
   xmlNanoHTTPClose(context);
   if (!packet)
   {
      printf("Unable to fetch the cover from amazon.com\n");
      return NULL;
   }

   //Save the file and return the path
   cover_path = _eclair_cover_build_cover_path(eclair, artist, album);
   cover_file = fopen(cover_path, "wb");
   if (!cover_file)
   {
      free(cover_path);
      return NULL;
   }
   fwrite(cover, cover_size, sizeof(char), cover_file);
   fclose(cover_file);

   return cover_path;
}

//Build the cover file path from artist and album
//The returned path has to be freed by the caller
static char *_eclair_cover_build_cover_path(Eclair *eclair, const char *artist, const char *album)
{
   char *path;

   if (!eclair || !artist || !album)
      return NULL;

   //TODO: forbidden chars for filename?
   path = (char *)malloc(strlen(eclair->config.covers_dir_path) + strlen(artist) + strlen(album) + 6);
   path[0] = 0;
   strcat(path, eclair->config.covers_dir_path);
   strcat(path, artist);
   strcat(path, "_");
   strcat(path, album);
   strcat(path, ".jpg");

   return path;
}

//Build an URL to make a search on amazon.com webservices
static void _eclair_cover_build_url_for_search(const char *keywords, char url[MAX_URL_SIZE])
{
   url[0] = 0;
   strcat(url, "http://");
   strcat(url, amazon_hostname);
   strcat(url, "/onca/xml?Service=AWSECommerceService&SubscriptionId=");
   strcat(url, amazon_license_key);
   strcat(url, "&Operation=ItemSearch&Keywords=");
   strcat(url, keywords);
   strcat(url, "&SearchIndex=Music");   
}

//Build an URL to get images of the item corresponding to the ASIN
static void _eclair_cover_build_url_for_item_images(const char *ASIN, char url[MAX_URL_SIZE])
{
   url[0] = 0;
   strcat(url, "http://");
   strcat(url, amazon_hostname);
   strcat(url, "/onca/xml?Service=AWSECommerceService&SubscriptionId=");
   strcat(url, amazon_license_key);
   strcat(url, "&Operation=ItemLookup&ItemId=");
   strcat(url, ASIN);
   strcat(url, "&ResponseGroup=Images");   
}

//Convert a string to URL format (eg. 'space' -> "%20"...)
//The returned string has to be freed by the caller
static char *_eclair_cover_convert_to_url_format(const char *string)
{
   char *new_string;
   int i, j, string_length;

   if (!string)
      return NULL;

   string_length = strlen(string);
   new_string = (char *)malloc(string_length * 3 * sizeof(char) + 1);

   for (i = 0, j = 0; i < string_length; i++, j++)
   {
      if (string[i] == ' ' || string[i] == ';'  || string[i] == '&'  || string[i] == '='  ||
         string[i] == '<'  || string[i] == '>'  || string[i] == '?'  || string[i] == '@'  ||
         string[i] == '\"' || string[i] == '['  || string[i] == '#'  || string[i] == '\\' ||
         string[i] == '$'  || string[i] == ']'  || string[i] == '%'  || string[i] == '^'  ||
         string[i] == '\'' || string[i] == '¨'  || string[i] == '+'  || string[i] == '{'  ||
         string[i] == ','  || string[i] == '|'  || string[i] == '/'  || string[i] == '}'  ||
         string[i] == ':'  || string[i] == '~')
      {
         new_string[j] = '\%';
         sprintf(&new_string[j + 1], "%x", string[i]);
         j += 2;
      }
      else
         new_string[j] = string[i];
   }
   new_string[j] = 0;

   return new_string;
}

//Get the packet sent by the context and its length
//Return 0 if failed. The packet will have to freed by the caller
static Evas_Bool _eclair_cover_receive_next_packet(void *context, char **packet, int *length)
{
   char packet_chunk[PACKET_CHUNK_SIZE + 1];
   int num_bytes_read, pos;
   Eclair_Packet_Chunk *chunk;
   Evas_List *l, *chunks = NULL;

   if (!context || !packet || !length)
      return 0;

   *length = 0;
   while ((num_bytes_read = xmlNanoHTTPRead(context, packet_chunk, PACKET_CHUNK_SIZE)) > 0)
   {
      chunk = (Eclair_Packet_Chunk *)malloc(sizeof(Eclair_Packet_Chunk));
      chunk->size = num_bytes_read;
      chunk->data = (char *)malloc(num_bytes_read * sizeof(char));
      memcpy(chunk->data, packet_chunk, num_bytes_read);
      chunks = evas_list_append(chunks, chunk);
      *length += num_bytes_read;
   }

   if (*length <= 0)
   {
      *length = 0;
      *packet = NULL;
      return 0;
   }

   pos = 0;
   *packet = (char *)malloc(*length * sizeof(char));
   for (l = chunks; l; l = l->next)
   {
      if (!(chunk = (Eclair_Packet_Chunk *)l->data))
         continue;
      memcpy(*packet + pos, chunk->data, chunk->size);
      free(chunk->data);
      pos += chunk->size;
      free(chunk);
   }
   evas_list_free(chunks);

   return 1;
}

//Search recursively the node called prop in the xml tree and return its value
static xmlNode *_eclair_cover_get_node_xml_tree(xmlNode *root_node, const char *prop)
{
   xmlNode *n, *children_node;

   for (n = root_node; n; n = n->next)
   {
      if (n->type == XML_ELEMENT_NODE)
      {
         if (xmlStrcmp((const xmlChar *)prop, n->name) == 0)
            return n;
      }
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
