#ifndef EMPHASIS_COVER_H_
#define EMPHASIS_COVER_H_

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>

#define PACKET_CHUNK_SIZE 1024
#define MAX_REQUEST_SIZE 1024
#define NUM_EXTENSIONS 3
#define NUM_COVER_NAMES 2

char *emphasis_cover_file_get_from_amazon(const char *artist,
                                          const char *album, 
                                          int *size);
char *emphasis_cover_add_uri_special_chars(const char *uri);
void _emphasis_cover_build_url_for_item_search(const char *keywords,
                                               char url[MAX_REQUEST_SIZE]);
void _emphasis_cover_build_url_for_item_images(const char *ASIN,
                                               char url[MAX_REQUEST_SIZE]);
int _emphasis_cover_fetch(const char *url, char **data);
int _emphasis_cover_connect_to_hostname(const char *hostname);
int _emphasis_cover_connect_to_host(const struct hostent *host);
int _emphasis_cover_receive_next_packet(int socket_fd, 
                                        char **packet,
                                        int *length);
int _emphasis_cover_extract_http_body(char *packet, 
                                      int packet_length,
                                      char **body);
xmlNode *_emphasis_cover_get_node_xml_tree(xmlNode * root_node,
                                           const char *prop);
xmlChar *_emphasis_cover_get_prop_value_from_xml_tree(xmlNode *
                                                      root_node,
                                                      const char *prop);

const char *emphasis_cover_haricotmagique(void);

#endif /* EMPHASIS_COVER_H_ */
