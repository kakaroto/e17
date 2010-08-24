#ifndef IRC_PARSE_H
# define IRC_PARSE_H

# include "Emote.h"

void irc_parse_input(char *input, const char *server, Emote_Protocol *m);

#endif
