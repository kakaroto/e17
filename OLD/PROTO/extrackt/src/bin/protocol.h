#ifndef _EX_PROTOCOL_H
#define _EX_PROTOCOL_H

void	ex_protocol_parse(Extrackt *ex,char *msg, int length);    
void	ex_protocol_send(int fd, char *header, char *data);
int	ex_protocol_receive(Extrackt *ex);

#endif
