#ifndef _EX_MAIN_H
#define _EX_MAIN_H

Extrackt *extrackt_init(void);
void      extrackt_shutdown(Extrackt *ex);
void      extrackt_parse(Extrackt *ext, char **options);

#endif    
