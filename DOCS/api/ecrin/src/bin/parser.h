#ifndef __PARSER_H__
#define __PARSER_H__


#include <libxml/xpath.h>


int  ecrin_parse_init     (char *filename);

void ecrin_parse_shutdown (void);

void ecrin_parse_define   (xmlNodePtr memberdef);

void ecrin_parse_enum     (xmlNodePtr memberdef);

void ecrin_parse_xml      (void);



#endif /* __PARSER_H__ */
