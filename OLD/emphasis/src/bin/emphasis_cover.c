#include "emphasis.h"
#include "emphasis_cover.h"

/*********************************************
 * Code took from eclair_cover               *
 * Thanks to MoOm for his code and agreement *
 *********************************************/

const char *amazon_hostname = "webservices.amazon.com";
const char *amazon_license_key = "1C2JV3FG53J7GNV7RP82";
const char *cover_extensions[NUM_EXTENSIONS] = { "jpg", "jpeg", "png" };
const char *cover_names[NUM_COVER_NAMES] = { "cover", "folder" };

typedef struct _Emphasis_Cover_Packet_Chunk Emphasis_Cover_Packet_Chunk;
struct _Emphasis_Cover_Packet_Chunk
{
  int size;
  char *data;
};

//Fetch the cover corresponding to artist and album, and return the cover
//NULL if failed. The returned data will have to be freed if success
char *
emphasis_cover_file_get_from_amazon(const char *artist, 
                                    const char *album,
                                    int *size)
{
  char *body;
  int body_length;
  xmlDocPtr doc;
  xmlNode *image_node;
  xmlChar *ASIN, *cover_url;
  char url[MAX_REQUEST_SIZE];
  char *keywords, *converted_keywords;

  if (!artist || !album || (strlen(artist) <= 0) || (strlen(album) <= 0))
    return NULL;

  //Get the ASIN of the album
  keywords = malloc(strlen(artist) + strlen(album) + 2);
  sprintf(keywords, "%s %s", artist, album);
  converted_keywords = emphasis_cover_add_uri_special_chars(keywords);
  free(keywords);

  _emphasis_cover_build_url_for_item_search(converted_keywords, url);
  free(converted_keywords);

  body_length = _emphasis_cover_fetch(url, &body);
  if (body_length <= 0)
    {
      fprintf(stderr, "Cover: Unable to download cover from amazon.com\n");
      return NULL;
    }

  doc = xmlReadMemory(body, body_length, "noname.xml", NULL, 0);
  free(body);
  if (!doc)
    {
      fprintf(stderr, "Cover: Amazon webservices sent a non xml response\n");
      return NULL;
    }
  ASIN =
    _emphasis_cover_get_prop_value_from_xml_tree(xmlDocGetRootElement(doc),
                                                 "ASIN");
  xmlFreeDoc(doc);
  if (!ASIN)
    {
      fprintf(stderr,
              "Cover: Unable to find the item \"%s - %s\" on amazon\n",
              artist, album);
      return NULL;
    }


  //Get cover url from the ASIN
  _emphasis_cover_build_url_for_item_images((const char *) ASIN, url);
  xmlFree(ASIN);
  body_length = _emphasis_cover_fetch(url, &body);
  if (body_length <= 0)
    {
      fprintf(stderr, "Cover: Unable to download cover from amazon.com\n");
      return NULL;
    }

  doc = xmlReadMemory(body, body_length, "noname.xml", NULL, 0);
  free(body);
  if (!doc)
    {
      fprintf(stderr,
              "Cover: Amazon.com webservices sent a non xml response\n");
      return NULL;
    }
  if (!
      (image_node =
       _emphasis_cover_get_node_xml_tree(xmlDocGetRootElement(doc),
                                         "LargeImage")))
    {
      if (!
          (image_node =
           _emphasis_cover_get_node_xml_tree(xmlDocGetRootElement(doc),
                                             "MediumImage")))
        image_node =
          _emphasis_cover_get_node_xml_tree(xmlDocGetRootElement(doc),
                                            "SmallImage");
    }
  cover_url = _emphasis_cover_get_prop_value_from_xml_tree(image_node, "URL");
  xmlFreeDoc(doc);
  if (!cover_url)
    {
      fprintf(stderr, "Cover: Unable to find the cover for %s - %s\n", artist,
              album);
      return NULL;
    }

  body_length = _emphasis_cover_fetch((const char *) cover_url, &body);
  xmlFree(cover_url);
  if (body_length <= 0)
    {
      fprintf(stderr, "Cover: Unable to download cover from amazon.com\n");
      return NULL;
    }

  *size = body_length;

  return body;
}

//Add the uri special chars (e.g. ' ' -> "%20")
//The returned string has to be freed
char *
emphasis_cover_add_uri_special_chars(const char *uri)
{
  int uri_length;
  char *new_uri;
  int i, j;

  if (!uri)
    return NULL;

  uri_length = strlen(uri);
  new_uri = malloc(uri_length *3 + 1);

  for (i = 0, j = 0; i < uri_length; i++, j++)
    {
      if (uri[i] == ' ' || uri[i] == ';' || uri[i] == '&' || uri[i] == '='
          || uri[i] == '<' || uri[i] == '>' || uri[i] == '?' || uri[i] == '@'
          || uri[i] == '\"' || uri[i] == '[' || uri[i] == '#' || uri[i] == '\\'
          || uri[i] == '$' || uri[i] == ']' || uri[i] == '%' || uri[i] == '^'
          || uri[i] == '\'' || uri[i] == '+' || uri[i] == '{' || uri[i] == ':'
          || uri[i] == ',' || uri[i] == '|' || uri[i] == '/' || uri[i] == '}'
          || uri[i] == '~')
        {
          new_uri[j] = '%';
          sprintf(&new_uri[j + 1], "%2x", uri[i]);
          j += 2;
        }
      else
        new_uri[j] = uri[i];
    }
  new_uri[j] = 0;

  return new_uri;
}

//Build a request to make a search on amazon.com webservices
void
_emphasis_cover_build_url_for_item_search(const char *keywords,
                                          char url[MAX_REQUEST_SIZE])
{
  sprintf(url,
          "http://%s/onca/xml?Service=AWSECommerceService&SubscriptionId=%s"
          "&Operation=ItemSearch&Keywords=%s&SearchIndex=Music",
          amazon_hostname, amazon_license_key, keywords);
}

//Build an request to get images of the item corresponding to the ASIN
void
_emphasis_cover_build_url_for_item_images(const char *ASIN,
                                          char url[MAX_REQUEST_SIZE])
{
  sprintf(url,
          "http://%s/onca/xml?Service=AWSECommerceService&SubscriptionId=%s"
          "&Operation=ItemLookup&ItemId=%s&ResponseGroup=Images",
          amazon_hostname, amazon_license_key, ASIN);
}

//Try to fetch the file stored at url on host he (may be NULL) and put the data into data
//data has to be freed if success
//Return the length of the data (negative if failed) 
int
_emphasis_cover_fetch(const char *url, char **data)
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
  if ((socket_fd = _emphasis_cover_connect_to_hostname(host)) < 0)
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
  _emphasis_cover_receive_next_packet(socket_fd, &packet, &packet_length);
  body_length =
    _emphasis_cover_extract_http_body(packet, packet_length, &body);
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

//Try to connect to an hostname
//Return the file descriptor of the socket created (need to be destroyed with close())
//-1 if failed
int
_emphasis_cover_connect_to_hostname(const char *hostname)
{
  struct hostent *he;

  if (!hostname)
    return 0;

  if (!(he = gethostbyname(hostname)))
    {
      fprintf(stderr, "Cover: Unable to find IP of %s\n", hostname);
      return -1;
    }
  return _emphasis_cover_connect_to_host(he);
}

//Try to connect to an host
//Return the file descriptor of the socket created 
//(need to be destroyed with close())
//-1 if failed
int
_emphasis_cover_connect_to_host(const struct hostent *host)
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
  if (connect(socket_fd, (struct sockaddr *) &adress, sizeof(struct sockaddr))
      != 0)
    {
      fprintf(stderr, "Cover: Unable to connect to %s\n",
              inet_ntoa(adress.sin_addr));
      close(socket_fd);
      return -1;
    }

  return socket_fd;
}

//Get the packet sent by the context and its length
//Return 0 if failed. The packet will have to freed by the caller
int
_emphasis_cover_receive_next_packet(int socket_fd, char **packet, int *length)
{
  char packet_chunk[PACKET_CHUNK_SIZE];
  int num_bytes_read, pos;
  Emphasis_Cover_Packet_Chunk *chunk;
  Eina_List *l, *chunks = NULL;

  if (socket_fd < 0 || !packet || !length)
    return 0;

  *length = 0;
  while ((num_bytes_read =
          read(socket_fd, packet_chunk, PACKET_CHUNK_SIZE)) > 0)
    {
      chunk = malloc(sizeof(Emphasis_Cover_Packet_Chunk));
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
int
_emphasis_cover_extract_http_body(char *packet, int packet_length,
                                  char **body)
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

//Search recursively the node called prop in the xml tree and return its value
xmlNode *
_emphasis_cover_get_node_xml_tree(xmlNode * root_node, const char *prop)
{
  xmlNode *n, *children_node;

  for (n = root_node; n; n = n->next)
    {
      if (n->type == XML_ELEMENT_NODE
          && xmlStrcmp((xmlChar *) prop, n->name) == 0)
        return n;
      if ((children_node =
           _emphasis_cover_get_node_xml_tree(n->children, prop)))
        return children_node;
    }

  return NULL;
}

//Search the node recursively called prop in the xml tree and return its value
xmlChar *
_emphasis_cover_get_prop_value_from_xml_tree(xmlNode * root_node,
                                             const char *prop)
{
  xmlNode *node;

  if (!(node = _emphasis_cover_get_node_xml_tree(root_node, prop)))
    return NULL;
  else
    return xmlNodeGetContent(node);
}


/* TODO : documentation */
const char *
emphasis_cover_haricotmagique(void)
{
  char *path;
  int id;

  srand(time(NULL));
  id = rand() % 4;

  switch(id)
    {
    case 0:
      path = PACKAGE_DATA_DIR "/images/haricot_musique-amor.png";
      break;
    case 1:
      path = PACKAGE_DATA_DIR "/images/haricot_musique-grrrr.png";
      break;
    case 2:
      path = PACKAGE_DATA_DIR "/images/haricot_musique-lalaa.png";
      break;
    case 3:
      path = PACKAGE_DATA_DIR "/images/haricot_musique-mario.png";
      break;
    default:
      path = PACKAGE_DATA_DIR "/images/haricot_musique-mooo.png";
    }

  return path;
}
